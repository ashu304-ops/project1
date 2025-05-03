# Use an official Python runtime as a parent image
FROM python:3.9-slim

# Set the working directory in the container
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

# Install necessary Python dependencies
RUN pip install --no-cache-dir numpy scipy

# Set the command to run the Python script
CMD ["python", "damping_optimization.py"]
