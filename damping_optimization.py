import numpy as np
from scipy.integrate import odeint

# System parameters
m = 1.0  # mass
k = 20.0  # spring constant
c_values = np.linspace(0.1, 20, 20)  # possible damping values (discrete)

# Q-learning parameters
n_actions = len(c_values)
q_table = np.zeros(n_actions)
alpha = 0.1      # learning rate
gamma = 0.9      # discount factor
epsilon = 0.2    # exploration rate
episodes = 100   # training episodes

# Simulate system for given damping
def simulate(c):
    def dynamics(y, t):
        x, v = y
        dxdt = [v, -(c/m)*v - (k/m)*x]
        return dxdt

    y0 = [1.0, 0.0]  # initial condition
    t = np.linspace(0, 10, 500)
    sol = odeint(dynamics, y0, t)
    x = sol[:, 0]

    overshoot = np.max(x)
    settled = np.all(np.abs(x[-50:]) < 0.05)
    reward = -overshoot
    if settled:
        reward += 5
    return reward

# Training loop
for episode in range(episodes):
    if np.random.rand() < epsilon:
        action = np.random.randint(n_actions)  # explore
    else:
        action = np.argmax(q_table)            # exploit

    c = c_values[action]
    reward = simulate(c)
    q_table[action] += alpha * (reward + gamma * np.max(q_table) - q_table[action])

# Output best damping coefficient
best_index = np.argmax(q_table)
best_c = c_values[best_index]
print(f"Best damping coefficient: {best_c:.2f}")

# testing 

# ---- Testing phase ----
def test_system(c):
    def dynamics(y, t):
        x, v = y
        dxdt = [v, -(c/m)*v - (k/m)*x]
        return dxdt

    y0 = [1.0, 0.0]
    t = np.linspace(0, 10, 500)
    sol = odeint(dynamics, y0, t)
    x = sol[:, 0]

    overshoot = np.max(x)
    settled = np.all(np.abs(x[-50:]) < 0.05)

    print(f"\n--- Test Results ---")
    print(f"Tested damping coefficient: {c:.2f}")
    print(f"Overshoot: {overshoot:.3f}")
    print(f"Settled (last 1s): {'Yes' if settled else 'No'}")

# Run test with best damping value
test_system(best_c)
