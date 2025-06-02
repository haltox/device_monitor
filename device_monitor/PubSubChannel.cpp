#include "PubSubChannel.h"

PubSubChannel::~PubSubChannel()
{
	{
		std::scoped_lock<std::mutex> lock{ mutex };
		pendingDestruction.store(true);
	}

	while (pendingNotify.load() > 0) {
		std::this_thread::sleep_for(std::chrono::nanoseconds{10});
	}
}

PubSubChannel::SubscriptionId
PubSubChannel::Subscribe(Callback cb)
{
	std::scoped_lock<std::mutex> lock{ mutex };
	SubscriptionId id = ++idCursor;
	pendingSubscriptions.push_back({ id, cb });

	return id;
}

void PubSubChannel::Unsubscribe(SubscriptionId id)
{
	std::scoped_lock<std::mutex> lock{ mutex };
	pendingUnsubscriptions.push_back(id);
}

void PubSubChannel::ClearSubscriptions()
{
	std::scoped_lock<std::mutex> lock{ mutex };
	for (auto& [id, cb] : subscriptions) {
		pendingUnsubscriptions.push_back(id);
	}
}

void PubSubChannel::Notify()
{
	pendingNotify.fetch_add(1);

	{
		std::scoped_lock<std::mutex> lock{ mutex };
	
		if (pendingDestruction) {
			pendingNotify.fetch_sub(1);
			return;
		};

		ResolvePendingOperations();
	}

	for (auto& [id, cb] : subscriptions) {
		cb();
	}

	pendingNotify.fetch_sub(1);
}

void PubSubChannel::ResolvePendingOperations()
{
	ResolvePendingUnsubscribes();
	ResolvePendingSubscribes();
}

void 
PubSubChannel::ResolvePendingUnsubscribes()
{
	for (auto id : pendingUnsubscriptions) {
		auto subIt = subscriptions.find(id);
		if (subIt != subscriptions.end()) {
			subscriptions.erase(subIt);
		}
	}
	pendingUnsubscriptions.clear();
}

void
PubSubChannel::ResolvePendingSubscribes()
{
	for (auto& [id, cb] : pendingSubscriptions) {
		subscriptions[id] = cb;
	}
	pendingSubscriptions.clear();
}
