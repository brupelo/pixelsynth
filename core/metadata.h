#pragma once
#include "static.h"

BEGIN_NAMESPACE(Core)

class PropertyMetadata
{
	class Data
	{
		Hash hash_;
		std::string title_;
		PropertyValue defaultValue_;

		friend class PropertyMetadata;
		friend class Builder;
	};
public:
	explicit PropertyMetadata(const Data&& data)
		: data_(data)
	{
	}

	Hash hash() const noexcept { return data_.hash_; }
	std::string title() const noexcept { return data_.title_; }
	PropertyValue defaultValue() const noexcept { return data_.defaultValue_; }

	class Builder
	{
	public:
		explicit Builder(const char* title) { data_.hash_ = Core::hash(title); data_.title_ = title; }
		PropertyMetadataPtr build() noexcept { return std::make_shared<PropertyMetadata>(std::move(*this)); }

		template <typename T>
		Builder&& ofType() { data_.defaultValue_ = T(); return std::move(*this); }

	private:
		friend class PropertyMetadata;
		Data data_;
	};

	PropertyMetadata(Builder&& rhs)
		: data_(std::move(rhs.data_))
	{}

private:
	const Data data_;
};

class ConnectorMetadata
{
	class Data
	{
		Hash hash_;
		std::string title_;
		ConnectorType type_;

		friend class ConnectorMetadata;
		friend class Builder;
	};
public:
	explicit ConnectorMetadata(const Data&& data)
		: data_(data)
	{
	}

	Hash hash() const noexcept { return data_.hash_; }
	std::string title() const noexcept { return data_.title_; }
	ConnectorType type() const noexcept { return data_.type_; }

	class Builder
	{
	public:
		Builder(const char* title, ConnectorType type) { data_.hash_ = Core::hash(title); data_.title_ = title; data_.type_ = type; }
		ConnectorMetadataPtr build() noexcept { return std::make_shared<ConnectorMetadata>(std::move(*this)); }

	private:
		friend class ConnectorMetadata;
		Data data_;
	};

	ConnectorMetadata(Builder&& rhs)
		: data_(std::move(rhs.data_))
	{}

private:
	const Data data_;
};

struct Metadata
{
	PropertyMetadataCollection propertyMetadataCollection;
	ConnectorMetadataCollection connectorMetadataCollection;
};

END_NAMESPACE(Core)

namespace std
{
	template<> struct hash<Core::ConnectorMetadata>
	{
		size_t operator()(const Core::ConnectorMetadata& s) const
		{
			auto h1(hash<string>()(s.title()));
			auto h2(hash<Core::ConnectorType>()(s.type()));
			return h1 ^ (h2 << 1);
		}
	};

	template<> struct hash<Core::ConnectorMetadataCollection>
	{
		size_t operator()(const Core::ConnectorMetadataCollection& s) const
		{
			size_t h = 54059;
			for (auto&& item : s) h ^= hash<Core::ConnectorMetadata>()(*item);
			return h;
		}
	};
}
