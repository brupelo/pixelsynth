#pragma once

#include <bandit/bandit.h>
#include <tree/tree_util.h>
#include <core/project.h>
#include <core/node.h>
#include <core/metadata.h>

inline std::string title(std::shared_ptr<const Core::Node> node)
{
	auto prop = node->prop(Core::hash("Title"));
	if (!prop) return "";
	return prop->get<std::string>(0);
}

inline std::shared_ptr<const Core::Node> findNode(Core::Project& project, std::string nodeTitle)
{
	auto result = std::find_if(begin(project.current().nodes()), end(project.current().nodes()), [nodeTitle](auto& node)
	{
		return title(node) == nodeTitle;
	});
	if (result == end(project.current().nodes())) return std::shared_ptr<const Core::Node>();
	return *result;
}

inline std::shared_ptr<const Core::Node> outputNode(std::shared_ptr<const Core::Connection> connection)
{
	return std::get<0>(connection->connection());
}

inline const Core::ConnectorMetadata* connector(std::shared_ptr<const Core::Node> node, std::string connectorTitle)
{
	auto result = find_if(begin(node->connectorMetadata()), end(node->connectorMetadata()), [connectorTitle](auto& metadata)
	{
		return metadata.hash() == Core::hash(connectorTitle.c_str());
	});
	if (result == end(node->connectorMetadata())) throw new std::logic_error("Unknown connector");
	return &(*result);
}

inline std::ostream& operator<<(std::ostream &strm, Core::NodePtr &a) {
	return strm << "Node(" << title(a) << ")";
}