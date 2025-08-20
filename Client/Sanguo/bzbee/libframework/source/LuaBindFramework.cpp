/*!
 * \file LuaBindFramework.cpp
 *
 * \author
 * \date
 *
 * 
 */

#include "LuaBindFramework.h"
#include "CustomHeader.h"

// declaretion c functions

// source: ../client/bzbee/libframework/source/Framework.cpp
NS::Framework* get_framework(void);

// source: ../client/bzbee/libframework/source/action/ActionComponent.cpp
float sine_in(float f);
float sine_out(float f);
float sine(float f);
float quadratic_in(float f);
float quadratic_out(float f);
float quadratic(float f);
float cubic_in(float f);
float cubic_out(float f);
float cubic(float f);
float quartic_in(float f);
float quartic_out(float f);
float quartic(float f);
float quintic_in(float f);
float quintic_out(float f);
float quintic(float f);
float circular_in(float f);
float circular_out(float f);
float circular(float f);
float exponential_in(float f);
float exponential_out(float f);
float exponential(float f);
float ease_back_in(float f);
float ease_back_out(float f);
float ease_back(float f);
float elastic_in(float f);
float elastic_out(float f);
float elastic(float f);
float bounce_in(float f);
float bounce_out(float f);
float bounce(float f);


bool LuaBindFramework::bindCFunction(LuaPlus::LuaObject& luaObj)
{

    // source: ../client/bzbee/libframework/source/Framework.cpp
    luaObj.RegisterDirect("get_framework", get_framework);

    // source: ../client/bzbee/libframework/source/action/ActionComponent.cpp
    luaObj.RegisterDirect("sine_in", sine_in);
    luaObj.RegisterDirect("sine_out", sine_out);
    luaObj.RegisterDirect("sine", sine);
    luaObj.RegisterDirect("quadratic_in", quadratic_in);
    luaObj.RegisterDirect("quadratic_out", quadratic_out);
    luaObj.RegisterDirect("quadratic", quadratic);
    luaObj.RegisterDirect("cubic_in", cubic_in);
    luaObj.RegisterDirect("cubic_out", cubic_out);
    luaObj.RegisterDirect("cubic", cubic);
    luaObj.RegisterDirect("quartic_in", quartic_in);
    luaObj.RegisterDirect("quartic_out", quartic_out);
    luaObj.RegisterDirect("quartic", quartic);
    luaObj.RegisterDirect("quintic_in", quintic_in);
    luaObj.RegisterDirect("quintic_out", quintic_out);
    luaObj.RegisterDirect("quintic", quintic);
    luaObj.RegisterDirect("circular_in", circular_in);
    luaObj.RegisterDirect("circular_out", circular_out);
    luaObj.RegisterDirect("circular", circular);
    luaObj.RegisterDirect("exponential_in", exponential_in);
    luaObj.RegisterDirect("exponential_out", exponential_out);
    luaObj.RegisterDirect("exponential", exponential);
    luaObj.RegisterDirect("ease_back_in", ease_back_in);
    luaObj.RegisterDirect("ease_back_out", ease_back_out);
    luaObj.RegisterDirect("ease_back", ease_back);
    luaObj.RegisterDirect("elastic_in", elastic_in);
    luaObj.RegisterDirect("elastic_out", elastic_out);
    luaObj.RegisterDirect("elastic", elastic);
    luaObj.RegisterDirect("bounce_in", bounce_in);
    luaObj.RegisterDirect("bounce_out", bounce_out);
    luaObj.RegisterDirect("bounce", bounce);

    return true;
}

// declaretion class member functions
static void bindActionComponent(LuaPlus::LuaObject& meta, LuaBind::LuaMethodSet& methodSet)
{
    if (methodSet.insert("isActive").second) meta.RegisterObjectDirect("isActive", reinterpret_cast<ActionComponent*>(0), &ActionComponent::isActive);
    if (methodSet.insert("stop").second) meta.RegisterObjectDirect("stop", reinterpret_cast<ActionComponent*>(0), &ActionComponent::stop);
}

static void bindComponent(LuaPlus::LuaObject& meta, LuaBind::LuaMethodSet& methodSet)
{
    if (methodSet.insert("addComponent").second) meta.RegisterObjectDirect("addComponent", reinterpret_cast<Component*>(0), &Component::addComponent);
    if (methodSet.insert("findComp").second) meta.RegisterObjectDirect("findComp", reinterpret_cast<Component*>(0), &Component::findComp);
    if (methodSet.insert("getClassName").second) meta.RegisterObjectDirect("getClassName", reinterpret_cast<Component*>(0), &Component::getClassName);
    if (methodSet.insert("getComponentByIndex").second) meta.RegisterObjectDirect("getComponentByIndex", reinterpret_cast<Component*>(0), &Component::getComponentByIndex);
    if (methodSet.insert("getComponentByUniqueId").second) meta.RegisterObjectDirect("getComponentByUniqueId", reinterpret_cast<Component*>(0), &Component::getComponentByUniqueId);
    if (methodSet.insert("getComponentIndex").second) meta.RegisterObjectDirect("getComponentIndex", reinterpret_cast<Component*>(0), &Component::getComponentIndex);
    if (methodSet.insert("getComponentList").second) meta.RegisterObjectDirect("getComponentList", reinterpret_cast<Component*>(0), &Component::getComponentList);
    if (methodSet.insert("getEntity").second) meta.RegisterObjectDirect("getEntity", reinterpret_cast<Component*>(0), &Component::getEntity);
    if (methodSet.insert("getName").second) meta.RegisterObjectDirect("getName", reinterpret_cast<Component*>(0), &Component::getName);
    if (methodSet.insert("getNumComponent").second) meta.RegisterObjectDirect("getNumComponent", reinterpret_cast<Component*>(0), &Component::getNumComponent);
    if (methodSet.insert("getUniqueId").second) meta.RegisterObjectDirect("getUniqueId", reinterpret_cast<Component*>(0), &Component::getUniqueId);
    if (methodSet.insert("register_update").second) meta.RegisterObjectDirect("register_update", reinterpret_cast<Component*>(0), &Component::register_update);
    if (methodSet.insert("removeAllComponent").second) meta.RegisterObjectDirect("removeAllComponent", reinterpret_cast<Component*>(0), &Component::removeAllComponent);
    if (methodSet.insert("removeComponent").second) meta.RegisterObjectDirect("removeComponent", reinterpret_cast<Component*>(0), &Component::removeComponent);
    if (methodSet.insert("removeComponentByUniqueId").second) meta.RegisterObjectDirect("removeComponentByUniqueId", reinterpret_cast<Component*>(0), &Component::removeComponentByUniqueId);
    if (methodSet.insert("removeFromParent").second) meta.RegisterObjectDirect("removeFromParent", reinterpret_cast<Component*>(0), &Component::removeFromParent);
    if (methodSet.insert("setName").second) meta.RegisterObjectDirect("setName", reinterpret_cast<Component*>(0), &Component::setName);
    if (methodSet.insert("swapComponent").second) meta.RegisterObjectDirect("swapComponent", reinterpret_cast<Component*>(0), &Component::swapComponent);
}

static void bindRenderableComponent(LuaPlus::LuaObject& meta, LuaBind::LuaMethodSet& methodSet)
{
    if (methodSet.insert("setRenderOrder").second) meta.RegisterObjectDirect("setRenderOrder", reinterpret_cast<RenderableComponent*>(0), &RenderableComponent::setRenderOrder);
    if (methodSet.insert("setRenderable").second) meta.RegisterObjectDirect("setRenderable", reinterpret_cast<RenderableComponent*>(0), &RenderableComponent::setRenderable);
}



bool LuaBindFramework::bindMemberFunction()
{
    // class binding
    LuaBind::addClassBindingFunc("ActionComponent", bindActionComponent);
    LuaBind::addClassBindingFunc("Component", bindComponent);
    LuaBind::addClassBindingFunc("RenderableComponent", bindRenderableComponent);

	return true;
}
