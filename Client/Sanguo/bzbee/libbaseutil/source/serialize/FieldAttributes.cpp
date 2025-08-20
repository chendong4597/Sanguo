/*!
 * \file FieldAttributes.cpp
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 04/14/2016
 *
 * 
 */
#include "FieldAttributes.h"
#include <limits>

NS_BEGIN

const IFieldAttribute FFA_DISABLED(IFieldAttribute::ATTR_DISABLED);
const IFieldAttribute FFA_FIXED_SIZE(IFieldAttribute::ATTR_FIXED_SIZE);
const IFieldAttribute FFA_DONNOT_CREATE_PROP(IFieldAttribute::ATTR_DONNOT_CREATE_PROP);
const IntegerFieldAttribute FFA_DEFAULT_INTEGER(0, 0, 1);
const IntegerFieldAttribute FFA_DEFAULT_INTEGER_UNBOUND(std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 1);
const IntegerFieldAttribute FFA_DEFAULT_INTEGER_UNEXPAND(0, 0, 1, true, false, 0);
const IntegerFieldAttribute FFA_DEFAULT_INTEGER_1_100_1(1, 100, 1);
const IntegerFieldAttribute FFA_DEFAULT_INTEGER_DEGREE(0, 359, 1, true, true);
const IntegerFieldAttribute FFA_DEFAULT_COLOR_CHANNEL(0, 255, 1);
const FloatFieldAttribute FFA_DEFAULT_FLOAT(0.0, 0.0, 1.0);

const IntegerFieldAttribute FFA_POSITIVE_INTEGER(0, std::numeric_limits<int>::max(), 1);
const FloatFieldAttribute FFA_FLOAT_STEP_01(0.0, 0.0, 0.1);
const FloatFieldAttribute FFA_FLOAT_STEP_001(0.0, 0.0, 0.01);
const FloatFieldAttribute FFA_FLOAT_STEP_0001(0.0, 0.0, 0.001);
const FloatFieldAttribute FFA_FLOAT_0_1_STEP_001(0.0, 1.0, 0.01);
const FloatFieldAttribute FFA_DEFAULT_SCALE(-100.0f, 100.0, 0.01);

const IFieldAttribute FFA_PIECE_INFO_LIST_LOCAL(0, IFieldAttribute::SubType::PieceInfoListLocal);
const IFieldAttribute FFA_PIECE_INFO_LIST_GLOBAL(0, IFieldAttribute::SubType::PieceInfoListGlobal);
const IFieldAttribute FFA_COLOR_INFO_LIST_LOCAL(0, IFieldAttribute::SubType::ColorInfoListLocal);
const IFieldAttribute FFA_COLOR_INFO_LIST_GLOBAL(0, IFieldAttribute::SubType::ColorInfoListGlobal);
const IFieldAttribute FFA_STYLE_LIST_LOCAL(0, IFieldAttribute::SubType::StyleListLocal);
const IFieldAttribute FFA_STYLE_LIST_GLOBAL(0, IFieldAttribute::SubType::StyleListGlobal);
const IFieldAttribute FFA_FONT_FILE(0, IFieldAttribute::SubType::FontList);

const FileFieldAttribut FFA_SPRITE_FILE("./sprites", "Sprite File (*.xml;*.spr)|*.xml;*.spr");
const FileFieldAttribut FFA_PARTICLE_FILE("./particles", "particle File (*.ptc)|*.ptc");
const FileFieldAttribut FFA_MODEL_FILE("./models", "model File (*.Model)|*.model");
const FileFieldAttribut FFA_TEXTURE_FILE("./texture", "model File (*.png)|*.png");
const FileFieldAttribut FFA_SPINE_FILE("./spines", "Spine File (*.json)|*.json");


const ColorFieldAttribute FFA_DEFAULT_COLOR(false);
const ColorFieldAttribute FFA_ALPHA_COLOR(true);

NS_END
