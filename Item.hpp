#pragma once

#include "cinder/gl/gl.h"

#include "Math.h"
#include "Node.hpp"

class Item;
typedef std::unique_ptr<Item> ItemBox;
typedef std::shared_ptr<Item> ItemRef;

class Item {
public:
	Item() {}
	Item(gl::BatchRef theBatch) : mBatch(theBatch) {}

	Node & node() { return mNode; }
	void update(vec3 p, quat o, vec3 s) { mNode.setAll(p, o, s); };

	void setBatch(gl::BatchRef newBatch) { mBatch = newBatch; }

	void draw() {
		gl::ScopedModelMatrix scpMat;
		gl::multModelMatrix(mNode.matrix());
		mBatch->draw();
	}

	static ItemRef create(gl::BatchRef theBatch) { return std::make_shared<Item>(theBatch); };

	Node mNode;
	gl::BatchRef mBatch;
};