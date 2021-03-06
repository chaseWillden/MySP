#include "Array.h"
#include "Nil.h"
#include "Number.h"
#include "Object.h"
#include "String.h"
#include "../Executor.h"
#include <iostream>


using namespace std;


/****************************************************************************************
 *
 ****************************************************************************************/
Object::Object(Visibility visibility, ClassDefinition* cd)
    : Variable(visibility)
{
    //Save the class definition
    this->classDef = cd;

    //Create Object Properties
    this->properties = map<string, Variable**>();
    map<string, Variable> vars;
    map<string, Variable>::iterator it;
    Variable** vPointer = NULL;
    Variable* var;
    Visibility vis;

    ClassDefinition* c = cd;

    while (c != NULL) {
        vars = c->getProperties();
        for (it = vars.begin(); it != vars.end(); it++) {
            //Don't include private properties
            if (c != cd && it->second.getVisibility() == PRIVATE) {
                vis = INHERIT_PRIVATE;
            } else {
                vis = it->second.getVisibility();
            }
            var = new Variable(vis);
            vPointer = new Variable*;
            *vPointer = var;
            var->setPointer(vPointer);
            this->properties[it->first] = vPointer;
        }
        c = c->getInheritedClass();
    }
}


/****************************************************************************************
 *
 ****************************************************************************************/
Object::Object(Visibility visibility, Object* obj)
    : Variable(visibility)
{
    //Save the class definition
    this->classDef = obj->classDef;

    //Create Object Properties
    this->properties = map<string, Variable**>();
    map<string, Variable**> vars = obj->properties;
    map<string, Variable**>::iterator it;
    Variable** vPointer = NULL;
    Variable* var;
    for (it = vars.begin(); it != vars.end(); it++) {
        vPointer = new Variable*;
        var = Executor::makeVariableCopy((*it->second), (*it->second)->getVisibility());
        *vPointer = var;
        var->setPointer(vPointer);
        this->properties[it->first] = vPointer;
    }
}


/****************************************************************************************
 *
 ****************************************************************************************/
Object::~Object() {
    map<string, Variable**>::iterator it;
    for (it = this->properties.begin(); it != this->properties.end(); it++) {
        delete (*it->second);
        delete it->second;
    }
}


/****************************************************************************************
 *
 ****************************************************************************************/
char Object::getType() {
    return 'o';
}


/****************************************************************************************
 *
 ****************************************************************************************/
string Object::getTypeString() {
    return "object";
}

/*/////////////////////////////////////// !! ////////////////////////////////////////////
 *
 * FILE:
 *     Object.cpp
 *
 * DESCRIPTION:
 *     Object variable type
 *
 * AUTHOR:
 *     Jason Mace
 *
 *
 * Copyright 2015 by Jason Mace
 *
 */////////////////////////////////////// !! ////////////////////////////////////////////


/****************************************************************************************
 *
 ****************************************************************************************/
double Object::getNumberValue() {
    return 0;
}


/****************************************************************************************
 *
 ****************************************************************************************/
string Object::getStringValue() {
    return "OBJECT";
}


/****************************************************************************************
 *
 ****************************************************************************************/
bool Object::getBooleanValue() {
    return true;
}


/****************************************************************************************
 *
 ****************************************************************************************/
Variable* Object::getProperty(string index) {
    if (this->properties.find(index) == this->properties.end()) {
        throw RuntimeError("Dynamic property '"+index+"' of class does not exist", ERROR);
    } else if ((*this->properties[index])->getVisibility() == INHERIT_PRIVATE) {
        throw RuntimeError("Dynamic property '"+index+"' of class is privately owned by parent", ERROR);
    }

    return *(this->properties[index]);
}


/****************************************************************************************
 *
 ****************************************************************************************/
Method* Object::getMethod(string index) {
    Method* m = this->classDef->getMethod(index);

    if (m == NULL) {
        throw RuntimeError("Method '"+index+"' of class does not exist", FATAL);
    }

    return m;
}


/****************************************************************************************
 *
 ****************************************************************************************/
ClassDefinition* Object::getClass() {
    return this->classDef;
}
