# project1
ashu
Deployment of Reinforcement agent (mechanical damper design) using docker,kubernetes and argocd 
Imagine you're on a swing at the playground. When you stop pumping your legs, the swing slows down and eventually stops, right? That slowing down happens because something is resisting the swinging motion. 
A mechanical damper is like that invisible helper that makes things slow down or stop when they’re moving too fast. It doesn’t stop things all at once — it just smooths out the motion, kind of like how your swing gently comes to a stop. 
People use dampers in things like cars and doors. For example: 
● In a car, a damper helps keep the ride from feeling bumpy. 
● On a door, it can stop it from slamming shut too fast. 
The mechanical damping coefficient (often written as c) is a value that tells us how much damping force is applied to slow down motion in a mechanical system. It's used in systems where things move or vibrate, and we want to control or reduce that motion. 
Common Uses of the Damping Coefficient: 
1. Vibration Control 
○ In machines, the damping coefficient helps reduce shaking or vibrating, making the machine run smoother and last longer. 
2. Car Suspensions 
○ In vehicles, damping keeps the ride comfortable. A higher or lower coefficient changes how "bouncy" or "firm" the ride feels. 
3. Building Design (Earthquake Engineering) 
○ In tall buildings or bridges, damping helps absorb shock during earthquakes or strong winds to keep structures safe. 
4. Mechanical Clocks or Instruments 
○ Damping ensures moving parts come to rest without wobbling too much, improving accuracy. 
5. Robotics and Automation
○ Helps control how fast arms and joints stop or settle after moving. 
The Formula Involving Damping Coefficient: 
In physics and engineering, damping force (F) is often given by: 
F=−c⋅v 
Where: 
● c = damping coefficient (units: N·s/m) 
● v= velocity 
● The minus sign means the force works against the motion. 
Reinforcement Agent (in machine learning): 
Imagine you're playing a video game where your goal is to catch coins and avoid monsters. Every time you catch a coin, you get a point. Every time you hit a monster, you lose points. 
Now imagine a robot playing the game. The robot is learning how to play by trying things, making mistakes, and getting rewards or penalties. 
That robot is the reinforcement agent! 
So what does the agent do? 
● It tries actions (like jumping or running). 
● It gets feedback (reward or punishment). 
● It learns from the feedback to get better over time. 
This kind of learning is called reinforcement learning.
Simple example: 
● Agent: A robot dog. 
● Goal: Learn to walk. 
● Each time it moves forward without falling, it gets a reward (like a "good job!"). ● If it falls, it gets punished (like "oops, try again"). 
● Over time, it figures out how to walk well! 
Design Reinforement agent for giving optimize value of damping coefficient import numpy as np 
from scipy.integrate import odeint 
# System parameters 
m = 1.0 # mass 
k = 20.0 # spring constant 
c_values = np.linspace(0.1, 20, 20) # possible damping values (discrete) 
# Q-learning parameters 
n_actions = len(c_values) 
q_table = np.zeros(n_actions) 
alpha = 0.1 # learning rate 
gamma = 0.9 # discount factor 
epsilon = 0.2 # exploration rate 
episodes = 100 # training episodes
# Simulate system for given damping 
def simulate(c): 
 def dynamics(y, t): 
 x, v = y 
 dxdt = [v, -(c/m)*v - (k/m)*x] 
 return dxdt 
 y0 = [1.0, 0.0] # initial condition 
 t = np.linspace(0, 10, 500) 
 sol = odeint(dynamics, y0, t) 
 x = sol[:, 0] 
 overshoot = np.max(x) 
 settled = np.all(np.abs(x[-50:]) < 0.05)  reward = -overshoot 
 if settled: 
 reward += 5 
 return reward 
# Training loop 
for episode in range(episodes): 
 if np.random.rand() < epsilon: 
 action = np.random.randint(n_actions) # explore  else: 
 action = np.argmax(q_table) # exploit
 c = c_values[action] 
 reward = simulate(c) 
 q_table[action] += alpha * (reward + gamma * np.max(q_table) - q_table[action]) 
# Output best damping coefficient 
best_index = np.argmax(q_table) 
best_c = c_values[best_index] 
print(f"Best damping coefficient: {best_c:.2f}") 
Output 
--- Test Results --- 
Tested damping coefficient: 1.15 
Overshoot: 1.000 
Settled (last 1s): Yes 
Deployment 
Dockerize the application by creating 
# Use an official Python runtime as a parent image 
FROM python:3.9-slim 
# Set the working directory in the container 
WORKDIR /app 
# Copy the current directory contents into the container at /app COPY . /app
# Install necessary Python dependencies 
RUN pip install --no-cache-dir numpy scipy 
# Set the command to run the Python script 
CMD ["python", "damping_optimization.py"] 
After creating dockerfile 
docker build -t q-learning-damper . 
docker run q-learning-damper 
Push this docker image to dockerhub 
Creating deployment yaml file of creating pod using this docker images 
apiVersion: apps/v1 
kind: Deployment 
metadata: 
name: learning-deployment 
spec: 
replicas: 1 
selector: 
 matchLabels: 
 app: q-learning 
template: 
 metadata: 
 labels: 
 app: q-learning 
 spec: 
 containers: 
 - name: damper 
 image: ashu304/damping_optimization.py # Replace with your Docker Hub image 
 imagePullPolicy: Always 
Then start minikube start 
kubectl apply -f deployment.yaml 
Pod will get deployed 
Checking output 
Kubectl logs <podname>
Argocd on minikube 
Initial create argocd application file on yaml format so we can changes in future also Argocd yaml file 
apiVersion: argoproj.io/v1alpha1 
kind: Application 
metadata: 
name: qlearning-app 
namespace: argocd # ArgoCD must watch this namespace 
spec: 
project: default 
source: 
 repoURL: 'https://github.com/ashu304-ops/project1.git'  targetRevision: HEAD 
 path: . 
 # You can specify a subfolder if your manifests are not in the root 
destination: 
 server: 'https://kubernetes.default.svc' 
 namespace: default 
syncPolicy: 
 automated: 
 prune: true 
 selfHeal: true 
 syncOptions: 
 - CreateNamespace=true # Optional: creates namespace if missing Before starting argocd kindy push every file on github
Introduction of argocd 
What is Argo CD? 
Imagine you're building LEGO castles and you keep all your castle designs in a book (your GitHub). Every time you want to build a castle, you look at the book and follow the instructions. 
Now imagine you have a robot helper named Argo. Argo is super smart — if you change something in your book, Argo automatically sees the update and goes to rebuild the LEGO castle exactly how you wanted — no yelling, no mess! 
That’s what Argo CD does, but for computer programs. 
In real life: 
● You keep your app designs (code, configuration) in GitHub. 
● Argo CD checks GitHub. 
● When something changes, Argo CD tells your computers (servers) to update the program automatically. 
It’s like: 
● GitHub = instruction book 
● Kubernetes = your LEGO play area 
● Argo CD = the robot that follows the book and keeps everything just right 
Argocd in minikube 
Create namespace of argocd 
kubectl create namespace argocd 
Intializing argocd on minikube 
kubectl apply -n argocd -f https://raw.githubusercontent.com/argoproj/argo-cd/stable/manifests/install.yaml
Username -admin and password generation use command below 
kubectl get secret argocd-initial-admin-secret -n argocd -o jsonpath="{.data.password}" | base64 -d 
Port forward the argocd gui on this port 
kubectl port-forward svc/argocd-server -n argocd 8080:443 
Access it through below 
argocd login localhost:8080 
Else 
argocd login 192.168.49.2:31801 --username admin --password <your-password> --insecure After login argocd run argocd yaml file 
Kubectl apply -f argocd.yaml 
You can see on argocd gui app named qlearning is deployed on argocd and tap on synced .
