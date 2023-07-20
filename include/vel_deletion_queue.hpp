#pragma once

#include <functional>
#include <deque>
#include <initializer_list>

namespace vel {

struct DeletionQueue {
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) {
		deletors.push_back(function);
	}

	void flush() {
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
			(*it)();
		}

		deletors.clear();
	}
};

#define VK_DELETE_PUSH(deletionQueue, ...) \
    deletionQueue.push_function([=]() {  __VA_ARGS__; })

}

