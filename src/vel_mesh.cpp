#include "../include/vel_mesh.hpp"

namespace vel {

VertexInputDescription Vertex::getVertexDescription() {
	VertexInputDescription description;

	VkVertexInputBindingDescription mainBinding{};
	mainBinding.binding = 0; // binding index
	mainBinding.stride = sizeof(Vertex); // it should jump sizeof(Vertex) bytes to get from one vertex to the next
	mainBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	description.bindings.push_back(mainBinding);

	VkVertexInputAttributeDescription positionAttribute{};
	positionAttribute.binding = 0; // set the data source
	positionAttribute.location = 0; // set a location
	positionAttribute.format = VK_FORMAT_R32G32B32_SFLOAT; // set a format (size)
	positionAttribute.offset = offsetof(Vertex, position); // tell the shader where position is located

	VkVertexInputAttributeDescription normalAttribute{};
	normalAttribute.binding = 0;
	normalAttribute.location = 1;
	normalAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	normalAttribute.offset = offsetof(Vertex, normal);

	VkVertexInputAttributeDescription colorAttribute{};
	colorAttribute.binding = 0;
	colorAttribute.location = 2;
	colorAttribute.format = VK_FORMAT_R32G32B32_SFLOAT;
	colorAttribute.offset = offsetof(Vertex, color);

	description.attributes.push_back(positionAttribute);
	description.attributes.push_back(normalAttribute);
	description.attributes.push_back(colorAttribute);

	return description;
}

}