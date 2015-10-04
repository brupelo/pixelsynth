#pragma once
#include "static.h"

BEGIN_NAMESPACE(Core)

struct MutationInfo
{
	static std::shared_ptr<MutationInfo> compare(const Document& prev, const Document& cur) noexcept;

	enum class ChangeType { Added, Removed, Mutated };

	template <typename T>
	struct Change
	{
		T prev;
		T cur;
		ChangeType type;
		NodePtr parent;
		size_t index;

		Change(const T prev, const T cur, ChangeType type, const NodePtr parent, size_t index)
			: prev(prev)
			, cur(cur)
			, type(type)
			, parent(parent)
			, index(index)
		{
		}

		friend bool operator==(const Change& lhs, const Change& rhs)
		{
			return lhs.prev == rhs.prev
				&& lhs.cur == rhs.cur
				&& lhs.type == rhs.type
				&& lhs.parent == rhs.parent
				&& lhs.index == rhs.index;
		}

		friend bool operator!=(const Change& lhs, const Change& rhs)
		{
			return !(lhs == rhs);
		}

		friend bool operator<(const Change& lhs, const Change& rhs)
		{
			return lhs.index < rhs.index;
		}

		friend bool operator<=(const Change& lhs, const Change& rhs)
		{
			return !(rhs < lhs);
		}

		friend bool operator>(const Change& lhs, const Change& rhs)
		{
			return rhs < lhs;
		}

		friend bool operator>=(const Change& lhs, const Change& rhs)
		{
			return !(lhs < rhs);
		}
	};

	template <typename T>
	using ChangeSet = std::set<Change<T>>;

	ChangeSet<NodePtr> nodes;
	ChangeSet<PropertyPtr> properties;
	ChangeSet<ConnectorMetadataPtr> connectors;
	ChangeSet<ConnectionPtr> connections;

	const Document& prev;
	const Document& cur;

	MutationInfo(const Document& prev, const Document& cur)
		: prev(prev),
		  cur(cur)
	{
	}
};

END_NAMESPACE(Core)