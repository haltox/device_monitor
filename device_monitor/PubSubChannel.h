#pragma once

#include <cstdint>
#include <map>
#include <list>
#include <functional>
#include <mutex>
#include <atomic>

class PubSubChannel
{
public:
	using SubscriptionId = uint32_t;
	using Callback = std::function<void()>;

	~PubSubChannel();

	SubscriptionId Subscribe(Callback cb);
	void Unsubscribe(SubscriptionId id);
	void ClearSubscriptions();
	void Notify();


private:
	void ResolvePendingOperations();
	void ResolvePendingUnsubscribes();
	void ResolvePendingSubscribes();

private:

	std::mutex mutex{};

	// Lol yeah - I know I know. From the outside, these modifications have already happened in non const functions
	// and the state of these members shouldn't be otherwise available beyond the api frontier.
	// It also makes sense to have notify being const while simultaneously having a mechanism that ensures
	// the modifications to the subscriptions do not happen while we are processing the callbacks.
	// Pending the modifications to the subscriptions is a good way to do this. Right before we process the callbacks
	// is the best moment to resolve the pending operations.
	// 
	// It's a mess but it's my mess and it's perfect just the way it is.
	std::map<SubscriptionId, Callback> subscriptions{};
	std::vector< std::pair<SubscriptionId, Callback> > pendingSubscriptions{};
	std::vector<SubscriptionId> pendingUnsubscriptions{};

	uint32_t idCursor{ 0 };

	std::atomic<int> pendingNotify{ 0 };
	std::atomic<bool> pendingDestruction{ false };
};

