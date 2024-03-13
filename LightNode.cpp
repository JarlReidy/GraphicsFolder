#include "stdafx.h"
#include "LightNode.h"
#include "Visitor.h"

// in implementation file to avoid circular depenency on header files
void LightNode::Accept(Visitor* v)
{
	v->Visit(this);
}
