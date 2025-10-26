/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma

#include <vector>

namespace rnoh {

template <typename PayloadT>
class Subject {
 public:
  using Shared = std::shared_ptr<Subject>;
  using Weak = std::weak_ptr<Subject>;

  class Observer {
   protected:
    Observer(Subject::Shared const& subject) : m_subject(subject) {
      subject->registerObserver(this);
    }

    // disallow copy and move to ensure `this` remains valid
    Observer(Observer const&) = delete;
    Observer& operator=(Observer const&) = delete;
    Observer(Observer&&) = delete;
    Observer& operator=(Observer&&) = delete;

    virtual ~Observer() {
      auto subject = m_subject.lock();
      if (subject) {
        subject->unregisterObserver(this);
      }
    };
    virtual void onUpdate(PayloadT const& payload) = 0;

   private:
    Subject::Weak m_subject;
    friend class Subject;
  };

 protected:
  void registerObserver(Observer* observer) {
    m_observers.push_back(observer);
  }

  void unregisterObserver(Observer* observer) {
    m_observers.erase(
        std::remove(m_observers.begin(), m_observers.end(), observer),
        m_observers.end());
  }

  void notifyObservers(PayloadT const& payload) {
    for (auto observer : m_observers) {
      observer->onUpdate(payload);
    }
  }

  std::vector<Observer*> m_observers;
};

} // namespace rnoh