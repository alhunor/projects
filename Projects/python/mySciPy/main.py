import numpy as np
import timeit
from scipy import ndimage
from scipy import misc
import matplotlib.pyplot as plt
import numpy.random as rand
from scipy.cluster import vq


def compare_ndarraytolist():
    t = timeit.timeit ("""
import numpy as np
arr = np.arange(1e7)
larr = arr.tolist()
    
def list_times(alist, scalar):
    for i, val in enumerate(alist):
        alist[i] = val * scalar
    return alist
    
list_times(larr, 1.1)
""", number=5)

    print (t)

    t = timeit.timeit ("""
import numpy as np
arr = np.arange(1e7)
arr*1.1
""", number=5)

    print (t)

#compare_ndarraytolist()

def createAndReshapeArrays():
    #Array creation
    alist = [1, 2, 3]
    arr = np.array(alist)
    arr2 = np.arange(1,4)
    arr3 = np.arange(3)
    arr4 = np.zeros(3, dtype = int)
    print (arr, arr2, arr3, arr4)

    arr = np.linspace(0,1,11)
    print (arr)
    arr = np.logspace(0,1,11, base = 10)
    print (arr)

    #reshape and flatten
    arr = np.arange(125)
    arr2 = np.reshape(arr, (5,5,5))
    print (arr2)
    arr3 = arr2.ravel()
    print (arr3)

#createAndReshapeArrays()

def recordArrys():
    recarr = np.zeros((2,), dtype = ('i4, f4, a10'))
    print (recarr)
    col1 = np.arange(1, 3, dtype = int)
    col2 = np.arange(2, dtype = np.float32)
    col3 = ['Hello', 'World']
    toadd = zip (col1, col2, col3)
    recarr[:] = list(toadd)
    print (recarr)
    print (recarr[0])
    recarr.dtype.names = ["id", "code", "text"]
    print (recarr["text"])

#recordArrys()

def indexingAndSlicing():
    alist = [[1,2],[3,4]]
    print (alist)
    print (alist[0][1]) # 2
    arr = np.array(alist, dtype = int)
    print (arr[0,1]) #2
    print (arr[:,1]) # last column [2,4]
    print (arr[1,:]) # bottom rom [3,4]
    print (arr[1]) # bottom rom [3,4]

    # where()
    arr = np.arange(10, dtype=int)
    index = np.where(arr<5)
    print (index)
    new_array = arr[index]
    print (new_array)
    index = arr<5
    print (index)

#indexingAndSlicing()


#Random numbers
def randomNumbers():
    np.random.seed(101)
    a = rand.random(100)
    print(a)

#Text files
def textFiles():
    np.random.seed(123)
    a = rand.random(100)
    a = (a*10).astype(int).reshape(10,10) # multiplies by 10, conversts to int, and reshapes to square
    print(a);
    np.savetxt("a.txt", a, "%i") # format string is needed to save as int
    b = np.loadtxt("a.txt").astype(int)
    print(b)

    #recArray files
    rec = np.loadtxt("recArray.txt", dtype= {'names':('Band', 'Title', 'Year'), 'formats':('a25', 'a25', 'i4')}, delimiter=',')
    print(rec['Band'])
    #for binary files use np.save(), np.savez()

#textFiles()

def images():
    img1 = np.zeros((20,20))+3
    img1[4:-4, 4:-4] = 6
    img1[7:-7, 7:-7] = 9
    plt.imshow(img1)
    plt.show()

    #f = misc.face()
    #plt.imsave('face.png', f)  # uses the Image module (PIL)

    img = plt.imread('face.png')
    #print (img)
    print (img[0,0,0])
    smallimg = img[:, :, 0]
    plt.imshow(smallimg)
    plt.show()

images()

"""
f = misc.face()
plt.imsave('face.png', f) # uses the Image module (PIL)

plt.imshow(f)
plt.show()
"""

def LinearAlgebra():
    # using matrices
    A = np.matrix([[3, 6, -5],
                   [1,-3,2],
                   [5,-1,4]])
    B = np.matrix ([12, -2, 10]).transpose()
    X = A ** (-1) * B
    print (X) # [1.75, 1.75, 0.75].transpose()

    #using ndarray
    A = np.array([[3, 6, -5],
                   [1,-3,2],
                   [5,-1,4]])
    B = np.array([12, -2, 10])
    X = np.linalg.inv(A).dot(B)
    print (X) # [1.75, 1.75, 0.75]

#LinearAlgebra()


#Data Modelling and Fitting

from scipy.optimize import curve_fit
def CurveFitting():
    def func(x,a,b):
        return a*x+b*b

    x=np.linspace(0,10,100)
    y = func(x, 1, 32)
    y2 = y + 0.9*np.random.normal(size = len(x)) # adding noise

    popt, pcov = curve_fit(func, x, y2)

    print (popt)
    perr = np.sqrt(np.diag(pcov))
    print (perr)

#CurveFitting()

def kmeansExample():
    c1 = np.random.randn(100,2)+5
    c2 = np.random.randn(30,2)-5
    c3 = np.random.randn(50,2)

    data = np.vstack([c1, c2, c3])

    centroids, variance = vq.kmeans(data, 3)
    # kmeans++ seeding is used
    identified, distance = vq.vq(data, centroids)

    vqc1 = data [identified == 0]
    vqc2 = data [identified == 1]
    vqc3 = data [identified == 2]

    print (identified)

    #print (vqc1)
    #print (vqc2)
    #print (vqc3)

    colors = identified / 3
    plt.scatter(data[:, 0], data[:, 1], c=colors, alpha=0.5 )
    plt.show()

#kmeansExample()

def timeseries():
    plt.figure(1)
    dw = np.random.randn(50,5)
    series = np.cumsum(dw, axis = 1)
    #plt.plot(series[:,1])
    #plt.plot(series)
    yticks = np.arange(5)
    colors = ['#624ea7', 'g', 'yellow', 'k', 'maroon']

    plt.barh(yticks, [7,1,6,2,4], align='center', color=colors)
    plt.yticks(yticks, ["Cat1", "Cat2", "Cat3", "Cat4","Cat5"])
    # Set tick colors:
    ax = plt.gca()
    ax.tick_params(axis='x', colors='blue')
    ax.tick_params(axis='y', colors='red')
    plt.xlabel('Horizontal axis')
    plt.ylabel('Vertical axis')
    plt.title('Title of the graph')
    plt.savefig("myfig.png")
    plt.show()

    plt.figure(2)
    people = ('Tom', 'Dick', 'Harry', 'Slim', 'Jim')
    y_pos = np.arange(len(people))
    performance = 3 + 5 * np.random.rand(len(people))
    error = np.random.rand(len(people))
    plt.barh(y_pos, performance, xerr=error, align='center', alpha=0.4)
    plt.yticks(y_pos, people)
    plt.xlabel('Performance')
    plt.title('How fast do you want to go today?')
    plt.show()


#timeseries()




