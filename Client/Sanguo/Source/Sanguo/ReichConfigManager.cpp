// Fill out your copyright notice in the Description page of Project Settings.


#include "ReichConfigManager.h"

ReichConfigManager& ReichConfigManager::getInstance()
{
	static ReichConfigManager s_ReichConfigManager;
	return s_ReichConfigManager;
}

ReichConfigManager::ReichConfigManager()
{
}

ReichConfigManager::~ReichConfigManager()
{
}




