#pragma once
#include <string>
#include <functional>

// Base command interface for the Command Pattern
class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
};

// Concrete command encapsulating plant grid placement execution
class PlantPlacementCommand : public ICommand {
private:
    std::function<void()> m_action;

public:
    explicit PlantPlacementCommand(std::function<void()> action)
        : m_action(action) {}

    void execute() override {
        if (m_action) {
            m_action();
        }
    }
};
