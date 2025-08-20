
#include "implicitRandomMapGenerator.h"

#include <numeric>
#include <array>
#include <algorithm>

namespace
{
	anl::CImplicitConstant Constant0{ 0.0 };
	anl::CImplicitConstant Constant1{ 1.0 };
}

namespace anl
{
	double CImplicitMultiSelect::get(double x, double y)
	{
		auto control = m_control.get(x, y);
		for (size_t i = 0; i < m_thresholds.size(); ++i)
		{
			if (control < m_thresholds[i].get(x, y))
			{
				return m_sources[i].get(x, y);
			}
		}

		return m_sources.back().get(x, y);
	}

	double CImplicitMultiSelect::get(double x, double y, double z)
	{
		return 0.0;
	}

	double CImplicitMultiSelect::get(double x, double y, double z, double w)
	{
		return 0.0;
	}

	double CImplicitMultiSelect::get(double x, double y, double z, double w, double u, double v)
	{
		return 0.0;
	}

	void CImplicitMultiSelect::setControlSource(double v)
	{
		m_control.set(v);
	}

	void CImplicitMultiSelect::setControlSource(CImplicitModuleBase *b)
	{
		m_control.set(b);
	}

	void CImplicitMultiSelect::setSources(const DoubleVec& s)
	{
		m_sources.clear();
		for (size_t i = 0; i < s.size(); i++)
		{
			m_sources.push_back(s[i]);
		}
	}

	void CImplicitMultiSelect::setSources(const CImplicitModuleBaseVec& s)
	{
		m_sources.clear();
		for (size_t i = 0; i < s.size(); i++)
		{
			m_sources.push_back(0.0);
			m_sources.back().set(s[i]);
		}
	}

	void CImplicitMultiSelect::setThreshold(const DoubleVec& t)
	{
		m_thresholds.clear();
		for (size_t i = 0; i < t.size(); i++)
		{
			m_thresholds.push_back(t[i]);
		}
	}

	void CImplicitMultiSelect::setThreshold(const CImplicitModuleBaseVec& t)
	{
		m_thresholds.clear();
		for (size_t i = 0; i < t.size(); i++)
		{
			m_thresholds.push_back(0.0);
			m_thresholds.back().set(t[i]);
		}
	}

	void map2DNoZ(int seamlessmode, CImplicitRandomMapGenerator &a, CImplicitModuleBase &m, SMappingRanges &ranges)
	{
		int w = a.m_width;
		int h = a.m_height;
		static double pi2 = 3.141592*2.0;

		for (int x = 0; x < w; ++x)
		{
			for (int y = 0; y < h; ++y)
			{
				double p = (double)x / (double)w;
				double q = (double)y / (double)h;
				double nx, ny, nz, nw, val = 0.0;
				double dx, dy;
				switch (seamlessmode)
				{
				case SEAMLESS_NONE:
				{
					nx = ranges.mapx0 + p * (ranges.mapx1 - ranges.mapx0);
					ny = ranges.mapy0 + q * (ranges.mapy1 - ranges.mapy0);
					val = m.get(nx, ny);
				} break;
				case SEAMLESS_X:
				{
					dx = ranges.loopx1 - ranges.loopx0;
					dy = ranges.mapy1 - ranges.mapy0;
					p = p * (ranges.mapx1 - ranges.mapx0) / (ranges.loopx1 - ranges.loopx0);
					nx = ranges.loopx0 + cos(p*pi2) * dx / pi2;
					ny = ranges.loopx0 + sin(p*pi2) * dx / pi2;
					nz = ranges.mapy0 + q * dy;
					val = m.get(nx, ny, nz);
				} break;
				case SEAMLESS_Y:
				{
					dx = ranges.mapx1 - ranges.mapx0;
					dy = ranges.loopy1 - ranges.loopy0;
					q = q * (ranges.mapy1 - ranges.mapy0) / (ranges.loopy1 - ranges.loopy0);
					nx = ranges.mapx0 + p * dx;
					ny = ranges.loopy0 + cos(q*pi2) * dy / pi2;
					nz = ranges.loopy0 + sin(q*pi2) * dy / pi2;
					val = m.get(nx, ny, nz);
				} break;

				case SEAMLESS_XY:
				{
					dx = ranges.loopx1 - ranges.loopx0;
					dy = ranges.loopy1 - ranges.loopy0;
					p = p * (ranges.mapx1 - ranges.mapx0) / (ranges.loopx1 - ranges.loopx0);
					q = q * (ranges.mapy1 - ranges.mapy0) / (ranges.loopy1 - ranges.loopy0);
					nx = ranges.loopx0 + cos(p*pi2) * dx / pi2;
					ny = ranges.loopx0 + sin(p*pi2) * dx / pi2;
					nz = ranges.loopy0 + cos(q*pi2) * dy / pi2;
					nw = ranges.loopy0 + sin(q*pi2) * dy / pi2;
					val = m.get(nx, ny, nz, nw);
				} break;
				default: break;
				}

				unsigned int value = static_cast<unsigned int>(val);
				a.m_data[y*a.m_width + x] = value;
			}
		}
	}


	CImplicitRandomMapGenerator::CImplicitRandomMapGenerator()
	{
		m_rnd = std::bind(std::uniform_int_distribution<>{}, m_rndEngine);
	}

	bool CImplicitRandomMapGenerator::resizeMap(int w, int h)
	{
		if (w <= 0 || h <= 0) { return false; }
		if (w == m_width && h == m_height) { return true; }

		m_width = w;
		m_height = h;

		m_data.resize(m_width*m_height);

		return true;
	}

	void CImplicitRandomMapGenerator::setSeed(unsigned seed)
	{
		m_seed = seed;
		m_rndEngine.seed(seed);
		m_rnd = std::bind(std::uniform_int_distribution<>{}, m_rndEngine);
	}

	bool CImplicitRandomMapGenerator::generate(double ground_line_perturb, double cave_frequency, const std::vector<double>& horizontal_rate, const std::vector<double>& vertical_rate,
		const std::vector<anl::CImplicitMultiSelect::CImplicitModuleBaseVec>& tiles)
	{
		if (m_width <= 0 || m_height <= 0) { return false; }
		const int strata_horizontal_count = horizontal_rate.size();
		const int strata_vertical_count = vertical_rate.size();
		if (horizontal_rate.empty() || vertical_rate.empty() || tiles.size() != strata_horizontal_count) { return false; }
		for (const auto& vertical_tiles : tiles)
		{
			if (vertical_tiles.size() != strata_vertical_count) { return false; }
		}

		double resolution = static_cast<double>(m_width) / m_height;

		// gradient
		anl::CImplicitGradient strata_horizontal;
		strata_horizontal.setGradient(-resolution, -resolution, -1, 1);
		anl::CImplicitCache strata_horizontal_cache;
		strata_horizontal_cache.setSource(&strata_horizontal);
		anl::CImplicitGradient strata_vertical;
		strata_vertical.setGradient(-resolution, resolution, -1, -1);
		anl::CImplicitCache strata_vertical_cache;
		strata_vertical_cache.setSource(&strata_vertical);

		// turbulence
		anl::CImplicitFractal fractal_horizontal{ anl::FBM, anl::GRADIENT, anl::QUINTIC };
		fractal_horizontal.setSeed(m_rnd());
		fractal_horizontal.setNumOctaves(6);
		fractal_horizontal.setFrequency(4);
		fractal_horizontal.setSeed(m_rnd());
		anl::CImplicitCache fractal_horizontal_cache;
		fractal_horizontal_cache.setSource(&fractal_horizontal);

		anl::CImplicitFractal fractal_vertical{ anl::HYBRIDMULTI, anl::GRADIENT, anl::QUINTIC };
		fractal_vertical.setSeed(m_rnd());
		fractal_vertical.setNumOctaves(6);
		fractal_vertical.setFrequency(1);
		fractal_vertical.setLacunarity(15);
		fractal_vertical.setGain(0.5);
		fractal_vertical.setSeed(m_rnd());

		anl::CImplicitCombiner combiner_perturb_horizontal{ anl::ADD };
		combiner_perturb_horizontal.setSource(0, &strata_horizontal_cache);
		anl::CImplicitAutoCorrect ac_perturb_horizontal{ 0.0, ground_line_perturb };
		ac_perturb_horizontal.setSource(&fractal_horizontal_cache);
		anl::CImplicitAutoCorrect ac_perturb_ground_line{ 0.0, ground_line_perturb };
		ac_perturb_ground_line.setSource(&fractal_horizontal_cache);
		anl::CImplicitSelect perturb_horizontal_select;
		perturb_horizontal_select.setControlSource(&strata_horizontal_cache);

		perturb_horizontal_select.setThreshold(0.2);
		perturb_horizontal_select.setLowSource(&ac_perturb_ground_line);
		perturb_horizontal_select.setHighSource(&ac_perturb_horizontal);
		combiner_perturb_horizontal.setSource(1, &perturb_horizontal_select);

		anl::CImplicitCache combiner_perturb_horizontal_cache;
		combiner_perturb_horizontal_cache.setSource(&combiner_perturb_horizontal);

		anl::CImplicitCombiner combiner_perturb_vertical{ anl::ADD };
		combiner_perturb_vertical.setSource(0, &strata_vertical_cache);
		anl::CImplicitAutoCorrect ac_perturb_vertical{ 0.0, 0.2 };
		ac_perturb_vertical.setSource(&fractal_vertical);
		combiner_perturb_vertical.setSource(1, &ac_perturb_vertical);
		anl::CImplicitScaleOffset scale_offset{ 1. / 1.2, 0 };
		scale_offset.setSource(&combiner_perturb_vertical);
		anl::CImplicitCache combiner_perturb_vertical_cache;
		combiner_perturb_vertical_cache.setSource(&scale_offset);

		// slice
		anl::CImplicitMultiSelect slice_horizontal;
		slice_horizontal.setControlSource(&combiner_perturb_horizontal_cache);
		double partition_param = (1.0 + ground_line_perturb);
		double horizontal_accumulated_rate{};
		anl::CImplicitMultiSelect::DoubleVec threshold_horizontal;
		for (int i = 0; i < strata_horizontal_count - 1; ++i)
		{
			horizontal_accumulated_rate += horizontal_rate[i];
			threshold_horizontal.push_back(horizontal_accumulated_rate);
		}
		slice_horizontal.setThreshold(threshold_horizontal);
		anl::CImplicitMultiSelect::DoubleVec source_horizontal;
		for (int i = 0; i < strata_horizontal_count; ++i)
		{
			source_horizontal.push_back(i * 1.);
		}
		slice_horizontal.setSources(source_horizontal);

		std::vector<anl::CImplicitMultiSelect> slice_vertical;
		slice_vertical.resize(strata_horizontal_count);
		anl::CImplicitMultiSelect::DoubleVec threshold_vertical;
		double vertical_accumulated_rate{};
		for (int i = 0; i < strata_vertical_count - 1; ++i)
		{
			vertical_accumulated_rate += vertical_rate[i];
			threshold_vertical.push_back(vertical_accumulated_rate);
		}
		for (int i = 0; i < strata_horizontal_count; ++i)
		{
			slice_vertical[i].setControlSource(&combiner_perturb_vertical_cache);
			slice_vertical[i].setThreshold(threshold_vertical);
			slice_vertical[i].setSources(tiles[i]);
		}

		anl::CImplicitMultiSelect slice;
		slice.setControlSource(&slice_horizontal);
		anl::CImplicitMultiSelect::DoubleVec thresold;
		for (int i = 0; i < strata_horizontal_count; ++i) {
			thresold.push_back(1 + i * 1.);
		}
		slice.setThreshold(thresold);
		anl::CImplicitMultiSelect::CImplicitModuleBaseVec source;
		for (auto& s : slice_vertical)
		{
			source.push_back(&s);
		}
		slice.setSources(source);

		// caves
		anl::CImplicitFractal cave_shape{ anl::FBM, anl::GRADIENT, anl::LINEAR };
		cave_shape.setSeed(m_rnd());
		cave_shape.setNumOctaves(6);
		cave_shape.setFrequency(cave_frequency);

		anl::CImplicitFractal cave_perturb{ anl::FBM, anl::GRADIENT, anl::NONE };
		cave_perturb.setSeed(m_rnd());
		cave_perturb.setNumOctaves(6);
		cave_perturb.setFrequency(3);

		anl::CImplicitScaleOffset cave_perturb_remap{ 0.0, 0.5 };
		cave_perturb_remap.setSource(&cave_perturb);
		anl::CImplicitTranslateDomain cave_translate_domain;
		cave_translate_domain.setSource(&cave_shape);
		cave_translate_domain.setYAxisSource(&cave_perturb_remap);

		//cavesSource
		anl::CImplicitMultiSelect cavesSource;
		cavesSource.setControlSource(&combiner_perturb_horizontal_cache);
		horizontal_accumulated_rate = 0.0;
		threshold_horizontal.clear();
		source_horizontal.clear();
		for (int i = 0; i < strata_horizontal_count - 1; ++i) {
			source_horizontal.push_back(i * 1.);
			horizontal_accumulated_rate += horizontal_rate[i];
			if (i == 0 && i + 1 < static_cast<int>(horizontal_rate.size()))
				threshold_horizontal.push_back(horizontal_accumulated_rate + horizontal_rate[i + 1] * 0.1);
			else
				threshold_horizontal.push_back(horizontal_accumulated_rate);
		} // End for
		source_horizontal.push_back(0);
		cavesSource.setThreshold(threshold_horizontal);
		cavesSource.setSources(source_horizontal);

		// cave bigger
		anl::CImplicitSelect cave_deeper;
		cave_deeper.setControlSource(&cavesSource);
		cave_deeper.setThreshold(2.0);
		cave_deeper.setLowSource(&Constant1);
		cave_deeper.setHighSource(&Constant0);
		anl::CImplicitSelect cave_bigger;
		cave_bigger.setControlSource(&cave_translate_domain);
		cave_bigger.setLowSource(&cave_deeper);
		cave_bigger.setHighSource(&Constant1);
		cave_bigger.setThreshold(-0.1);

		// cave shallower
		anl::CImplicitSelect cave_shallower;
		cave_shallower.setControlSource(&cavesSource);
		cave_shallower.setThreshold(1.0);
		cave_shallower.setLowSource(&Constant1);
		cave_shallower.setHighSource(&Constant0);
		anl::CImplicitSelect cave_smaller;
		cave_smaller.setControlSource(&cave_translate_domain);
		cave_smaller.setLowSource(&cave_shallower);
		cave_smaller.setHighSource(&Constant1);
		cave_smaller.setThreshold(-0.3);

		anl::CImplicitSelect cave_select;
		cave_select.setControlSource(&cavesSource);
		cave_select.setThreshold(2.0);
		cave_select.setLowSource(&cave_smaller);
		cave_select.setHighSource(&cave_bigger);

		anl::CImplicitCombiner combiner{ anl::MULT };
		combiner.setSource(0, &slice);
		combiner.setSource(1, &cave_select);

		anl::SMappingRanges ranges;
		ranges.mapx0 = -resolution;
		ranges.mapx1 = resolution;
		anl::map2DNoZ(anl::SEAMLESS_NONE, *this, combiner, ranges);

		return true;
	}

	bool CImplicitRandomMapGenerator::generate(const GeneratorInfo::VEC_LAYERINFO& layerInfos, const GeneratorInfo::VEC_DISTRIBUTIONINFO& distributionInfos)
	{
		if (m_width <= 0 || m_height <= 0)
			return false;

		NoiseFactory noiseFactory;
		static anl::CImplicitConstant const0{ 0.0 };

		//垂直方向深度图
		auto& controlVertical = noiseFactory.CreateNoise<anl::CImplicitGradient>();
		controlVertical.setSeed(m_rnd());
		controlVertical.setGradient(0.0, 0.0, -1.0, 1.0);

		// layer info to layer source and threshold
		anl::CImplicitMultiSelect::CImplicitModuleBaseVec layerSource;
		anl::CImplicitMultiSelect::CImplicitModuleBaseVec layerThreshold;
		layerSource.reserve(layerInfos.size() + 1);
		layerSource.push_back(&const0);
		layerThreshold.reserve(layerInfos.size());
		for (const auto& layerInfo : layerInfos) {
			const auto& perturbH = layerInfo.m_perturbH;

			// fractal
			auto& fractal = noiseFactory.CreateNoise<anl::CImplicitFractal>(anl::FBM, anl::GRADIENT, layerInfo.m_perturbH.m_interpType);
			fractal.setSeed(m_rnd());
			fractal.setNumOctaves(perturbH.m_numOctaves);
			fractal.setFrequency(perturbH.m_frequency);
			fractal.setGain(0.5);

			// autoCorrect
			auto lowCorrect = std::max(perturbH.m_high - perturbH.m_amplitude, 0.0);
			auto highCorrect = std::min(perturbH.m_high, 1.0);
			auto& autoCorrect = noiseFactory.CreateNoise<anl::CImplicitAutoCorrect>(lowCorrect, highCorrect);
			autoCorrect.setSource(&fractal);

			// constTile
			auto& constTile = noiseFactory.CreateNoise<anl::CImplicitConstant>(layerInfo.m_tile);

			layerSource.emplace_back(&constTile);
			layerThreshold.emplace_back(&autoCorrect);
		} // End for

		  //基于layer
		auto& baseLayer = noiseFactory.CreateNoise<anl::CImplicitMultiSelect>();
		baseLayer.setSeed(m_rnd());
		baseLayer.setControlSource(&controlVertical);
		baseLayer.setThreshold(layerThreshold);	//分界 0~1
		baseLayer.setSources(layerSource);

		//加上土块分布
		auto addDistribution = [&](anl::CImplicitModuleBase& source, const GeneratorInfo::DistributionInfo& distributionInfo) {
			// fractal
			auto& fractal = noiseFactory.CreateNoise<anl::CImplicitFractal>(anl::FBM, anl::GRADIENT, distributionInfo.m_interpType);
			fractal.setSeed(m_rnd());
			fractal.setNumOctaves(distributionInfo.m_numOctaves);
			fractal.setFrequency(distributionInfo.m_frequency);
			fractal.setGain(0.5);
			auto& autoCorrect = noiseFactory.CreateNoise<anl::CImplicitAutoCorrect>(0.0, 100.0);
			autoCorrect.setSource(&fractal);

			// perturbLow
			const auto& perturbLow = distributionInfo.m_perturbLow;
			auto& fractalLow = noiseFactory.CreateNoise<anl::CImplicitFractal>(anl::FBM, anl::GRADIENT, perturbLow.m_interpType);
			fractalLow.setSeed(m_rnd());
			fractalLow.setNumOctaves(perturbLow.m_numOctaves);
			fractalLow.setFrequency(perturbLow.m_frequency);
			fractalLow.setGain(0.5);
			auto lowCorrect = std::max(perturbLow.m_high - perturbLow.m_amplitude, 0.0);
			auto highCorrect = std::min(perturbLow.m_high, 1.0);
			auto& autoCorrectLow = noiseFactory.CreateNoise<anl::CImplicitAutoCorrect>(lowCorrect, highCorrect);
			autoCorrectLow.setSource(&fractalLow);

			// perturbHigh
			const auto& perturbHigh = distributionInfo.m_perturbHigh;
			auto& fractalHigh = noiseFactory.CreateNoise<anl::CImplicitFractal>(anl::FBM, anl::GRADIENT, perturbHigh.m_interpType);
			fractalHigh.setSeed(m_rnd());
			fractalHigh.setNumOctaves(perturbHigh.m_numOctaves);
			fractalHigh.setFrequency(perturbHigh.m_frequency);
			fractalHigh.setGain(0.5);
			lowCorrect = std::max(perturbHigh.m_high, 0.0);
			highCorrect = std::min(perturbHigh.m_high + perturbHigh.m_amplitude, 1.0);
			auto& autoCorrectHigh = noiseFactory.CreateNoise<anl::CImplicitAutoCorrect>(lowCorrect, highCorrect);
			autoCorrectHigh.setSource(&fractalHigh);

			// slice
			layerThreshold.clear();
			layerThreshold.reserve(2);
			layerThreshold.emplace_back(&autoCorrectLow);
			layerThreshold.emplace_back(&autoCorrectHigh);
			auto& slice = noiseFactory.CreateNoise<anl::CImplicitMultiSelect>();
			slice.setControlSource(&controlVertical);
			slice.setThreshold(layerThreshold);
			slice.setSources({ &const0, &autoCorrect, &const0 });

			// constTile
			auto& constTile = noiseFactory.CreateNoise<anl::CImplicitConstant>(distributionInfo.m_tile);

			auto& add = noiseFactory.CreateNoise<anl::CImplicitSelect>();
			add.setControlSource(&slice);
			add.setLowSource(&source);
			add.setHighSource(&constTile);
			add.setThreshold(distributionInfo.m_percentage);

			return &add;
		}; // End of addDistribution

		anl::CImplicitModuleBase* completeDistribution = &baseLayer;
		for (const auto& distributionInfo : distributionInfos) {
			completeDistribution = addDistribution(*completeDistribution, distributionInfo);
		} // End for

		  //天空保持干净
		auto& complete = noiseFactory.CreateNoise<anl::CImplicitSelect>();
		complete.setControlSource(&baseLayer);
		complete.setLowSource(&baseLayer);
		complete.setHighSource(completeDistribution);
		complete.setThreshold(0.1);

		double resolution = static_cast<double>(m_width) / m_height;
		anl::SMappingRanges ranges;
		ranges.mapx0 = -resolution;
		ranges.mapx1 = resolution;
		map2DNoZ(anl::SEAMLESS_NONE, *this, complete, ranges);

		return true;
	}
}
