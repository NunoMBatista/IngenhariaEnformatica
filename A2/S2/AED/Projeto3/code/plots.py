import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

def addLinearRegression(x_values, y_values):
    # Calculate the linear regression
    m, b = np.polyfit(x_values, y_values, 1)
    
    # Add the linear regression to the plot
    plt.plot(x_values, m*x_values + b, color="red")
    
    # Print the equation
    print(f"y = {m} * x + {b}")
    
    return m, b

def addQuadraticRegression(x_values, y_values):
    # Calculate the quadratic regression
    a, b, c = np.polyfit(x_values, y_values, 2)
    
    # Add the quadratic regression to the plot
    plt.plot(x_values, a*x_values**2 + b*x_values + c, color="red")
    
    # Print the equation
    print(f"y = {a} * x^2 + {b} * x + {c}")
    
    return a, b, c

def addNlogNregression(x_values, y_values):
    # Define the function to fit
    def func(x, a, b):
        return a*x*np.log(x) + b

    # Fit the function to the data
    popt, pcov = curve_fit(func, x_values, y_values)

    # Get the fitted parameters
    a, b = popt

    # Generate x values for the regression line
    x_line = np.linspace(min(x_values), max(x_values), 1000)

    # Calculate y values for the regression line
    y_line = func(x_line, a, b)

    # Add the regression line to the plot
    plt.plot(x_line, y_line, color="red")

    # Print the equation
    print(f"y = {a} * x * log(x) + {b}")

    return a, b

if __name__ == "__main__":
    # Read the csv file
    
    # Column 1: Increasing Order Times
    # Column 2: Decreasing Order Times
    # Column 3: Random Order Times

    # Each one of the 10 rows correspond to a different array size
    # 10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000
    
    data = np.genfromtxt("insertionSortTimes.csv", delimiter=",")
    
    x_values = np.array([10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000])
    
    plt.figure(1)
    # Plot the times for each order
    plt.scatter(x_values, data[:,0], label="Increasing Order")  
    plt.scatter(x_values, data[:,1], label="Decreasing Order")
    plt.scatter(x_values, data[:,2], label="Random Order")
    
    # Add the regressions for each order
    m1, b1 = addLinearRegression(x_values, data[:,0])
    m2, b2, c2 = addQuadraticRegression(x_values, data[:,1])
    m3, b3, c3 = addQuadraticRegression(x_values, data[:,2])   
    
    plt.xlabel("Array Size")
    plt.ylabel("Time (s)")
    plt.legend()
    
    plt.title("Insertion Sort")
    plt.show()
        
    # Now, in a different figure, plot the heap sort times
    data = np.genfromtxt("heapSortTimes.csv", delimiter=",")
    
    plt.figure(2)
    plt.scatter(x_values, data[:,0], label="Increasing Order")
    plt.scatter(x_values, data[:,1], label="Decreasing Order")
    plt.scatter(x_values, data[:,2], label="Random Order")
    
    m1, b1 = addNlogNregression(x_values, data[:,0])
    m2, b2 = addNlogNregression(x_values, data[:,1])
    m3, b3 = addNlogNregression(x_values, data[:,2])
    
    plt.xlabel("Array Size")
    plt.ylabel("Time (s)")
    plt.legend()
    
    plt.title("Heap Sort")
    plt.show()
    
    # Quick Sort
    data = np.genfromtxt("quickSortTimes.csv", delimiter=",")
    
    plt.figure(3)
    plt.scatter(x_values, data[:,0], label="Increasing Order")
    plt.scatter(x_values, data[:,1], label="Decreasing Order")
    plt.scatter(x_values, data[:,2], label="Random Order")

    m1, b1, c1 = addQuadraticRegression(x_values, data[:,0])
    m2, b2, c2 = addQuadraticRegression(x_values, data[:,1])
    m3, b3 = addNlogNregression(x_values, data[:,2])
    
    plt.xlabel("Array Size")
    plt.ylabel("Time (s)")
    plt.legend()
    
    plt.title("Quick Sort")
    
    plt.show()
    
    