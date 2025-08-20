/*!
 * \file implicitCImplicitRandomMapGenerator.h
 *
 * \author zhijieli
 * \date ���� 2016
 *
 * 
 */
#pragma once

#include "anl.h"

#include <array>
#include <vector>
#include <stdint.h>
#include <functional>
#include <random>
#include <memory>

class CImplicitRandomMapGenerator;
namespace anl
{
	class CImplicitModuleBase;
	struct SMappingRanges;
	void map2DNoZ(int seamlessmode, CImplicitRandomMapGenerator &a, CImplicitModuleBase &m, SMappingRanges &ranges);

	class CImplicitMultiSelect : public CImplicitModuleBase
	{
	public:
		using CImplicitModuleBaseVec = std::vector < CImplicitModuleBase* >;
		using DoubleVec = std::vector < double >;

		void setControlSource(double v);
		void setControlSource(CImplicitModuleBase *b);
		void setSources(const DoubleVec& s);
		void setSources(const CImplicitModuleBaseVec& s);
		void setThreshold(const DoubleVec& t);
		void setThreshold(const CImplicitModuleBaseVec& t);

		double get(double x, double y) override;
		double get(double x, double y, double z) override;
		double get(double x, double y, double z, double w) override;
		double get(double x, double y, double z, double w, double u, double v) override;

	protected:
		using CScalarParameterVec = std::vector < CScalarParameter >;

		CScalarParameter m_control{ 0.0 };
		CScalarParameterVec m_thresholds;
		CScalarParameterVec m_sources;
	};

	namespace GeneratorInfo
	{
		enum CONSTANT
		{
			MAX_LAYER = 10,
			MAX_DISTRIBUTION = 20,
		};

		class PerturbH
		{
		public:
			PerturbH() {}
			~PerturbH() {}
		public:
			unsigned int	m_interpType{ anl::QUINTIC };
			unsigned int	m_numOctaves{ 1 };
			double			m_high{ 0.0 };
			double			m_frequency{ 0.0 };
			double			m_amplitude{ 0.0 };
		}; // End of PerturbH

		class LayerInfo
		{
		public:
			LayerInfo() {}
			~LayerInfo() {}
		public:
			int				m_tile{ 0 };
			PerturbH		m_perturbH;
		}; // End of LayerInfo

		typedef std::vector<LayerInfo> VEC_LAYERINFO;

		class DistributionInfo
		{
		public:
			DistributionInfo() {}
			~DistributionInfo() {}
		public:
			int				m_tile{ 0 };
			unsigned int	m_interpType{ anl::LINEAR };
			unsigned int	m_numOctaves{ 1 };
			double			m_frequency{ 0.0 };
			double			m_percentage{ 0.0 };
			PerturbH		m_perturbLow;
			PerturbH		m_perturbHigh;
		}; // End of DistributionInfo

		typedef std::vector<DistributionInfo> VEC_DISTRIBUTIONINFO;
	} // End of GeneratorInfo

	class NoiseFactory
	{
		typedef std::shared_ptr<anl::CImplicitModuleBase> SPTR_MODULEBASE;
		typedef std::vector<SPTR_MODULEBASE> VEC_MODULEBASE;
	public:
		NoiseFactory() { m_buffers.clear(); }
		~NoiseFactory() { m_buffers.clear(); }
	public:
		template <typename TNoise, typename... TArg>
		TNoise& CreateNoise(TArg...);
	private:
		VEC_MODULEBASE m_buffers;
	}; // End of NoiseFactory

	template <typename TNoise, typename... TArg>
	TNoise& NoiseFactory::CreateNoise(TArg... args)
	{
		auto newNoise = std::make_shared<TNoise>(args...);
		m_buffers.emplace_back(newNoise);
		return *newNoise.get();
	} // End of CreateNoise for NoiseFactory

	class CImplicitRandomMapGenerator
	{
	public:
		CImplicitRandomMapGenerator();

		bool resizeMap(int w, int h);

		void setSeed(unsigned seed);

		bool generate(double ground_line_perturb, double cave_frequency, const std::vector<double>& horizontal_rate, const std::vector<double>& vertical_rate,
			const std::vector<anl::CImplicitMultiSelect::CImplicitModuleBaseVec>& tiles);

		bool generate(const GeneratorInfo::VEC_LAYERINFO& layerInfos, const GeneratorInfo::VEC_DISTRIBUTIONINFO& distributionInfos);

		const std::vector<unsigned int>& getData() const { return m_data; }
		int getWidth() const { return m_width; }
		int getHeight() const { return m_height; }

	private:
		friend void map2DNoZ(int seamlessmode, CImplicitRandomMapGenerator &a, CImplicitModuleBase &m, SMappingRanges &ranges);

	protected:
		std::default_random_engine m_rndEngine{};
		std::function<int()> m_rnd;
		int m_width{};
		int m_height{};
		std::vector<unsigned int> m_data;
		unsigned int m_seed;
	};
}
