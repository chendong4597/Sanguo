/*!
 * \file GlobalCfg.cpp
 *
 * \author cd
 * \date 2020/04/07
 *
 *
 */
#include "stdafx.h"
#include "RoleCreateCfg.h"
#include "XmlUtil.h"
#include "StringUtil.h"
#include "format.h"




RoleCreateCfg& RoleCreateCfg::getInstance()
{
	static RoleCreateCfg s_RoleCreateCfg;
	return s_RoleCreateCfg;
}

bool RoleCreateCfg::initializeCfg()
{

	tinyxml2::XMLDocument doc;
	if (!bzbee::XmlUtil::loadXmlFile(doc, "config/CreateRoleCfg.bin")) return false;

	return parseConfig(doc.RootElement());
}

void RoleCreateCfg::terminate()
{
	m_mapRoleInfo.clear();
}

bool RoleCreateCfg::parseConfig(tinyxml2::XMLElement* pXmlRoot)
{
	if (!pXmlRoot) { return false; }

	auto p = pXmlRoot->FirstChildElement("Table");
	if (!p) return false;

	auto pAttrXml = p->FirstChildElement("Attribute");
	while (pAttrXml)
	{
		std::string strKey;
		NS::XmlUtil::getAttribute(strKey, pAttrXml, "Name");
		if (strKey == "s_iconPath")
		{
			NS::XmlUtil::getAttribute(m_sIconPath, pAttrXml, "Value");
		}
		else if (strKey == "s_manHairMesh") {
			NS::XmlUtil::getAttribute(m_sManHairMeshPath, pAttrXml, "Value");
		}
		else if (strKey == "s_womanHairMesh") {
			NS::XmlUtil::getAttribute(m_sWomanHairMeshPath , pAttrXml, "Value");
		}
		else if (strKey == "s_manSkinM") {
			NS::XmlUtil::getAttribute(m_sManSkinM, pAttrXml, "Value");
		}
		else if (strKey == "s_womanSkinM") {
			NS::XmlUtil::getAttribute(m_sWomanSkinM, pAttrXml, "Value");
		}
		else if (strKey == "s_manMesh") {
			NS::XmlUtil::getAttribute(m_sManSkeletalMeshPath, pAttrXml, "Value");
		}
		else if (strKey == "s_womanMesh") {
			NS::XmlUtil::getAttribute(m_sWomanSkeletalMeshPath, pAttrXml, "Value");
		}
		else if (strKey == "s_manBPAniPath") {
			NS::XmlUtil::getAttribute(m_sManBPAniPath, pAttrXml, "Value");
		}
		else if (strKey == "s_womanBPAniPath") {
			NS::XmlUtil::getAttribute(m_sWomanBPAniPath, pAttrXml, "Value");
		}

		pAttrXml = pAttrXml->NextSiblingElement("Attribute");
	}
	auto xmlItem = p->FirstChildElement("Record");

	while (xmlItem != nullptr)
	{
		RoleCreateCfgCfgInfo info;
		NS::XmlUtil::getAttribute(info.n_id, xmlItem , "n_ID");
		NS::XmlUtil::getAttribute(info.n_sex, xmlItem, "n_Sex");
		NS::XmlUtil::getAttribute(info.n_type , xmlItem, "n_Type");
		NS::XmlUtil::getAttribute(info.n_nIdx, xmlItem, "n_Index");
		NS::XmlUtil::getAttribute(info.s_icon_path, xmlItem, "s_Icon");
		NS::XmlUtil::getAttribute(info.s_mesh_path, xmlItem, "s_Path");
		m_mapRoleInfo[info.n_id] = info;
		xmlItem = xmlItem->NextSiblingElement("Record");
	}
	return true;
}

const RoleCreateCfg::RoleCreateCfgCfgInfo* RoleCreateCfg::GetRoleInfo(int nSex , int nType, int nIndex)
{
	for (auto && item : m_mapRoleInfo)
	{
		if (item.second.n_sex == nSex && item.second.n_type == nType
			&& item.second.n_nIdx == nIndex)
		{
			return &(item.second);
		}
	}
	return nullptr;
}

std::string RoleCreateCfg::GetGameIconPath(std::string strIcon)
{
	std::string str = fmt::format("Texture2D'{0}/{1}.{2}'", m_sIconPath.c_str(), strIcon.c_str(), strIcon.c_str());
	return str;
}


std::string RoleCreateCfg::GetSkeletalMesh(int nSex, std::string strMesh)
{
	if (nSex == Sex_Tpy_Man)
	{
		return fmt::format("SkeletalMesh'{0}/{1}/{2}.{3}'", m_sManHairMeshPath.c_str(), strMesh.c_str(), strMesh.c_str(), strMesh.c_str());
	}
	return fmt::format("SkeletalMesh'{0}/{1}/{2}.{3}'", m_sWomanHairMeshPath.c_str(), strMesh.c_str(), strMesh.c_str(), strMesh.c_str());
}

std::string RoleCreateCfg::GetHairMaterial(int nSex, int nStyle)
{
	//Material'/Game/Player/man/mesh/hair/hair_01/M_hair.M_hair'
	if (nSex == Sex_Tpy_Man)
	{
		return fmt::format("Material'{0}/hair_{1:02d}/M_hair.M_hair'", m_sManHairMeshPath.c_str(), nStyle);
	}
	return fmt::format("Material'{0}/hair_{1:02d}/M_hair.M_hair'", m_sWomanHairMeshPath.c_str(), nStyle);
}


std::string RoleCreateCfg::GetHairColorTexture(int nSex, int nStyle, int nIndex)
{//Texture2D'/Game/Player/man/mesh/hair/hair_01/HairColor/man_hair_01_10.man_hair_01_10'
	if (nSex == Sex_Tpy_Man)
	{
		return fmt::format("Texture2D'{0}/hair_{1:02d}/HairColor/man_hair_{2:02d}_{3:02d}.man_hair_{4:02d}_{5:02d}'", m_sManHairMeshPath.c_str(), nStyle, nStyle, nIndex, nStyle, nIndex);
	}
	return fmt::format("Texture2D'{0}/hair_{1:02d}/HairColor/woman_hair_{2:02d}_{3:02d}.woman_hair_{4:02d}_{5:02d}'", m_sWomanHairMeshPath.c_str(), nStyle, nStyle, nIndex, nStyle, nIndex);
}

std::string RoleCreateCfg::GetSkinMaterial(int nSex, int nBodyTpy)
{//Material'/Game/Player/man/mesh/M_man_body_01.M_man_body_01'  Material'/Game/Player/woman/mesh/M_woman_body_01.M_woman_body_01'
	std::string strM;
	if (nSex == Sex_Tpy_Man)
	{
		strM = fmt::format("M_man_body_0{0}", nBodyTpy);
		return fmt::format("Material'/Game/Player/man/mesh/{0}.{1}'", strM.c_str(), strM.c_str());
	}
	else {
		strM = fmt::format("M_woman_body_0{0}", nBodyTpy);
		return fmt::format("Material'/Game/Player/woman/mesh/{0}.{1}'", strM.c_str(), strM.c_str());
	}
}


std::string RoleCreateCfg::GetSknColorTexture(int nSex, int nIndex)
{
	//Texture2D'/Game/Player/woman/mesh/SkinColor/woman_naked_diffuse_01.woman_naked_diffuse_01'
	if (nSex == Sex_Tpy_Man)
	{
		return fmt::format("Texture2D'{0}/man_naked_diffuse_{1:02d}.man_naked_diffuse_{2:02d}'", m_sManSkinM, nIndex, nIndex);
	}
	return fmt::format("Texture2D'{0}/woman_naked_diffuse_{1:02d}.woman_naked_diffuse_{2:02d}'", m_sWomanSkinM, nIndex, nIndex);
}

std::string RoleCreateCfg::GetMainSkeletalMesh(int nSex)
{
	if (nSex == Sex_Tpy_Man)
	{
		return fmt::format("SkeletalMesh'{0}'", m_sManSkeletalMeshPath);
	}
	return fmt::format("SkeletalMesh'{0}'", m_sWomanSkeletalMeshPath);
}

//蓝图加载路径最后一定要加_C
std::string RoleCreateCfg::GetBPAniPath(int nSex)
{
	if (nSex == Sex_Tpy_Man)
	{
		return fmt::format("AnimBlueprint'{0}_C'", m_sManBPAniPath);
	}
	return fmt::format("AnimBlueprint'{0}_C'", m_sWomanBPAniPath);
}


