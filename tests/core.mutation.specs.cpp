#include "static.h"
#include "mutationproject.h"

using namespace bandit;
using namespace Core;

template <typename T>
using Change = MutationInfo::Change<T>;
using ChangeType = MutationInfo::ChangeType;

go_bandit([]() {
	describe("mutation:", [&]()
	{
		std::unique_ptr<MutationProject> p;
		std::vector<std::shared_ptr<MutationInfo>> mutations;

		before_each([&]()
		{
			mutations.clear();

			p = std::make_unique<MutationProject>();
			p->setMutationCallback([&mutations](auto mutationInfo) { mutations.emplace_back(mutationInfo); });

			p->applyMutationsTo(MutationProject::NUM_MUTATIONS - 1);
		});

		it("should have emitted mutations", [&]()
		{
			AssertThat(mutations.size(), !Equals(0));
		});

		it("should emit added nodes", [&]()
		{
			auto mutation = mutations.at(0);
			AssertThat(mutation->nodes.size(), Equals(3));
			AssertThat(mutation->properties.size(), Equals(18));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(nullptr, p->a[0], ChangeType::Added, nullptr, p->root(), -1, 0)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(nullptr, p->b[0], ChangeType::Added, nullptr, p->root(), -1, 1)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(nullptr, p->c[0], ChangeType::Added, nullptr, p->root(), -1, 2)));
		});

		it("should emit removed nodes", [&]()
		{
			auto mutation = mutations.at(1);
			AssertThat(mutation->nodes.size(), Equals(2));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->b[0], nullptr, ChangeType::Removed, p->root(), nullptr, 1, -1)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[0], p->c[0], ChangeType::Mutated, p->root(), p->root(), 2, 1)));
		});

		it("should emit nodes re-added after undo", [&]()
		{
			auto mutation = mutations.at(2);
			AssertThat(mutation->nodes.size(), Equals(2));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(nullptr, p->b[0], ChangeType::Added, nullptr, p->root(), -1, 1)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[0], p->c[0], ChangeType::Mutated, p->root(), p->root(), 1, 2)));
		});

		it("should emit changed properties", [&]()
		{
			auto mutation = mutations.at(3);
			AssertThat(mutation->nodes.size(), Equals(1));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[0], p->a[3], ChangeType::Mutated, p->root(), p->root(), 0, 0)));

			AssertThat(mutation->properties.size(), Equals(5));
			AssertThat(mutation->properties, Contains(Change<PropertyPtr>(prop(*p->a[0], "int"), prop(*p->a[3], "int"), ChangeType::Mutated, p->a[0], p->a[3], 1, 1)));
			AssertThat(mutation->properties, Contains(Change<PropertyPtr>(prop(*p->a[0], "double"), prop(*p->a[3], "double"), ChangeType::Mutated, p->a[0], p->a[3], 2, 2)));
			AssertThat(mutation->properties, Contains(Change<PropertyPtr>(prop(*p->a[0], "vec2"), prop(*p->a[3], "vec2"), ChangeType::Mutated, p->a[0], p->a[3], 3, 3)));
			AssertThat(mutation->properties, Contains(Change<PropertyPtr>(prop(*p->a[0], "vec3"), prop(*p->a[3], "vec3"), ChangeType::Mutated, p->a[0], p->a[3], 4, 4)));
			AssertThat(mutation->properties, Contains(Change<PropertyPtr>(prop(*p->a[0], "string"), prop(*p->a[3], "string"), ChangeType::Mutated, p->a[0], p->a[3], 5, 5)));
		});

		it("should emit added connectors", [&]()
		{
			auto mutation = mutations.at(4);
			AssertThat(mutation->nodes.size(), Equals(1));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[3], p->a[4], ChangeType::Mutated, p->root(), p->root(), 0, 0)));

			AssertThat(mutation->connectors.size(), Equals(1));
			AssertThat(mutation->connectors, Contains(Change<ConnectorMetadataPtr>(nullptr, connector(*p->a[4], "Foo"), ChangeType::Added, nullptr, p->a[4], -1, 2)));
		});

		it("should emit removed connectors", [&]()
		{
			auto mutation = mutations.at(5);
			AssertThat(mutation->nodes.size(), Equals(1));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[4], p->a[5], ChangeType::Mutated, p->root(), p->root(), 0, 0)));

			AssertThat(mutation->connectors.size(), Equals(1));
			AssertThat(mutation->connectors, Contains(Change<ConnectorMetadataPtr>(connector(*p->a[4], "Foo"), nullptr, ChangeType::Removed, p->a[4], nullptr, 2, -1)));
		});

		it("should emit added connections", [&]()
		{
			auto mutation = mutations.at(6);
			AssertThat(mutation->connections.size(), Equals(1));
			AssertThat(mutation->connections.begin()->cur->connection(), Equals(make_tuple(p->a[6], connector(*p->a[6], "Out"), p->b[6], connector(*p->b[6], "In"))));
		});

		it("should emit removed connections", [&]()
		{
			auto mutation = mutations.at(7);
			AssertThat(mutation->connections.size(), Equals(1));
			AssertThat(mutation->connections.begin()->prev->connection(), Equals(make_tuple(p->a[6], connector(*p->a[6], "Out"), p->b[6], connector(*p->b[6], "In"))));
		});

		it("should emit reparenting from root to lower", [&]()
		{
			auto mutation = mutations.at(8);
			AssertThat(mutation->nodes.size(), Equals(3));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[8], p->a[8], ChangeType::Mutated, p->root(), p->c[8], 0, 1)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->b[8], p->b[8], ChangeType::Mutated, p->root(), p->c[8], 1, 0)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[8], p->c[8], ChangeType::Mutated, p->root(), p->root(), 2, 0)));
		});

		it("should emit reparenting from lower to root", [&]()
		{
			auto mutation = mutations.at(9);
			AssertThat(mutation->nodes.size(), Equals(3));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[8], p->a[8], ChangeType::Mutated, p->c[8], p->root(), 1, 0)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->b[8], p->b[8], ChangeType::Mutated, p->c[8], p->root(), 0, 1)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[8], p->c[8], ChangeType::Mutated, p->root(), p->root(), 0, 2)));
		});

		it("should emit setting a property to animated", [&]()
		{
			auto mutation = mutations.at(10);
			AssertThat(mutation->nodes.size(), Equals(1));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[9], p->a[10], ChangeType::Mutated, p->root(), p->root(), 0, 0)));
			AssertThat(mutation->properties.size(), Equals(1));
			AssertThat(mutation->properties, Contains(Change<PropertyPtr>(prop(*p->a[9], "double"), prop(*p->a[10], "double"), ChangeType::Mutated, p->a[9], p->a[10], 2, 2)));
		});

		it("should emit setting another property to animated", [&]()
		{
			auto mutation = mutations.at(11);
			AssertThat(mutation->nodes.size(), Equals(1));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[10], p->a[11], ChangeType::Mutated, p->root(), p->root(), 0, 0)));
			AssertThat(mutation->properties.size(), Equals(1));
			AssertThat(mutation->properties, Contains(Change<PropertyPtr>(prop(*p->a[10], "int"), prop(*p->a[11], "int"), ChangeType::Mutated, p->a[10], p->a[11], 1, 1)));
		});
	
		it("should emit inserting nodes", [&]()
		{
			auto mutation = mutations.at(12);
			AssertThat(mutation->nodes.size(), Equals(3));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(nullptr, p->between_ab[12], ChangeType::Added, nullptr, p->root(), -1, 1)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->b[12], p->b[12], ChangeType::Mutated, p->root(), p->root(), 1, 2)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[12], p->c[12], ChangeType::Mutated, p->root(), p->root(), 2, 3)));
		});
	
		it("should emit added properties", [&]()
		{
			auto mutation = mutations.at(14);
			AssertThat(mutation->nodes.size(), Equals(1));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[13], p->a[14], ChangeType::Mutated, p->root(), p->root(), 0, 0)));

			AssertThat(mutation->properties.size(), Equals(1));
			AssertThat(mutation->properties, Contains(Change<PropertyPtr>(nullptr, prop(*p->a[14], "new_property"), ChangeType::Added, nullptr, p->a[14], -1, 6)));
		});

		it("should emit removed properties", [&]()
		{
			auto mutation = mutations.at(15);
			AssertThat(mutation->nodes.size(), Equals(1));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[14], p->a[15], ChangeType::Mutated, p->root(), p->root(), 0, 0)));

			AssertThat(mutation->properties.size(), Equals(1));
			AssertThat(mutation->properties, Contains(Change<PropertyPtr>(prop(*p->a[14], "new_property"), nullptr, ChangeType::Removed, p->a[14], nullptr, 6, -1)));
		});
	
		it("should emit reparenting from root to lower on multiple levels", [&]()
		{
			auto mutation = mutations.at(18);
			AssertThat(mutation->nodes.size(), Equals(2));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->b[8], p->b[17], ChangeType::Mutated, p->root(), p->a[17], 1, 0)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[18], p->c[18], ChangeType::Mutated, p->root(), p->root(), 2, 1)));

			mutation = mutations.at(19);
			AssertThat(mutation->nodes.size(), Equals(2));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[17], p->a[18], ChangeType::Mutated, p->root(), p->c[17], 0, 0)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[18], p->c[18], ChangeType::Mutated, p->root(), p->root(), 1, 0)));
		});

		it("should properly undo/redo reparenting", [&]()
		{
			auto mutation = mutations.at(21);
			AssertThat(mutation->nodes.size(), Equals(2));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[17], p->a[18], ChangeType::Mutated, p->root(), p->c[17], 0, 0)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[18], p->c[18], ChangeType::Mutated, p->root(), p->root(), 1, 0)));
		});

		it("should properly undo/redo multiple mutations", [&]()
		{
			auto mutation = mutations.at(24);
			AssertThat(mutation->nodes.size(), Equals(4));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(nullptr, p->between_ab[24], ChangeType::Added, nullptr, p->root(), -1, 1)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(nullptr, p->between_ab2[24], ChangeType::Added, nullptr, p->root(), -1, 2)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->b[24], p->b[24], ChangeType::Mutated, p->root(), p->root(), 1, 3)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[24], p->c[24], ChangeType::Mutated, p->root(), p->root(), 2, 4)));

			mutation = mutations.at(25);
			AssertThat(mutation->nodes.size(), Equals(4));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->between_ab[24], nullptr, ChangeType::Removed, p->root(), nullptr, 1, -1)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->between_ab2[24], nullptr, ChangeType::Removed, p->root(), nullptr, 2, -1)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->b[24], p->b[24], ChangeType::Mutated, p->root(), p->root(), 3, 1)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[24], p->c[24], ChangeType::Mutated, p->root(), p->root(), 4, 2)));

			mutation = mutations.at(26);
			AssertThat(mutation->nodes.size(), Equals(4));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(nullptr, p->between_ab[24], ChangeType::Added, nullptr, p->root(), -1, 1)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(nullptr, p->between_ab2[24], ChangeType::Added, nullptr, p->root(), -1, 2)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->b[24], p->b[24], ChangeType::Mutated, p->root(), p->root(), 1, 3)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[24], p->c[24], ChangeType::Mutated, p->root(), p->root(), 2, 4)));
		});

		it("should propertly handle multiple reparentings", [&]()
		{
			auto mutation = mutations.at(27);
			AssertThat(mutation->nodes.size(), Equals(6));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[27], p->a[27], ChangeType::Mutated, p->root(), p->b[27], 0, 0)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[27], p->c[27], ChangeType::Mutated, p->root(), p->a[27], 4, 0)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->b[27], p->b[27], ChangeType::Mutated, p->root(), p->d[27], 3, 0)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(nullptr, p->d[27], ChangeType::Added, nullptr, p->root(), -1, 2)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->between_ab[27], p->between_ab[24], ChangeType::Mutated, p->root(), p->root(), 1, 0)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->between_ab2[27], p->between_ab2[24], ChangeType::Mutated, p->root(), p->root(), 2, 1)));
		});

		it("should propertly handle insert + move", [&]()
		{
			auto mutation = mutations.at(30);
			AssertThat(mutation->nodes.size(), Equals(4));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[30], p->a[30], ChangeType::Mutated, p->root(), p->root(), 0, 3)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->b[30], p->b[30], ChangeType::Mutated, p->root(), p->root(), 1, 2)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[30], p->c[30], ChangeType::Mutated, p->root(), p->root(), 2, 0)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(nullptr, p->d[30], ChangeType::Added, nullptr, p->root(), -1, 1)));
		});

		it("should propertly undo insert + move", [&]()
		{
			auto mutation = mutations.at(31);
			AssertThat(mutation->nodes.size(), Equals(4));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->a[30], p->a[30], ChangeType::Mutated, p->root(), p->root(), 3, 0)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->b[30], p->b[30], ChangeType::Mutated, p->root(), p->root(), 2, 1)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->c[30], p->c[30], ChangeType::Mutated, p->root(), p->root(), 0, 2)));
			AssertThat(mutation->nodes, Contains(Change<NodePtr>(p->d[30], nullptr, ChangeType::Removed, p->root(), nullptr, 1, -1)));
		});
	});
});