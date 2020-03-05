#pragma once

#include "AudioApiUtils.h"


#include <list>
// only prototype



class IAudioGraphNode {

	std::list<IAudioGraphNode*> inputs;
	std::list<IAudioGraphNode*> outputs;
public:
	void Propagate() {
		for (auto& input : inputs) {

		}
	}

	virtual void ApplyEffect() {

	}

	void AddSamples(void* data, size_t size) {

	}
};

class AudioGraph {

public:
	

};

