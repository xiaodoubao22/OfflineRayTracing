import matplotlib.pyplot as plt
import numpy as np

def drawSphere(ax, R, x, y, z):
    u = np.linspace(0, 2 * np.pi, 100)# 用参数方程画图
    v = np.linspace(0, np.pi, 100)
    x = R * np.outer(np.cos(u), np.sin(v)) + x# outer()外积函数：返回cosu × sinv
    y = R * np.outer(np.sin(u), np.sin(v)) + y# 
    z = R * np.outer(np.ones(np.size(u)), np.cos(v)) + z# ones()：返回一组[1,1,.......]

    ax.plot_surface(x, y, z, alpha=0.5)

myColor = [(1, 0, 0, 0.5), (0, 0, 1, 0.5), (0, 0.7, 0, 0.5), (0, 0.5, 0.5, 0.5), (1, 0.5, 0, 0.5), (0.5, 0.5, 0, 0.5), (0.5, 0, 1, 0.5)]
def getColor(i):
    global colorInd
    return myColor[i % len(myColor)]

if __name__ == '__main__':
    fig=plt.figure()
    ax=fig.add_subplot(projection='3d')
    # drawSphere(ax, 45.0, 350.0, 350.0, 80.0)
    drawSphere(ax, 0.25, 0.55, -0.75, 0.7)

    f = open("./SaveRay/Rays.txt")
    print("open")
    lines = f.readlines()
    for line in lines:
        strs = line.split()
        if len(strs) == 0:
            continue
        tag = strs[0]
        strs.pop(0)
        rayList = [float(x) for x in strs]
        if tag == 'pnt':
            ax.scatter3D(rayList[0], rayList[1], rayList[2], color=getColor(int(rayList[3])))
        if tag == 'ray':
            ax.quiver(rayList[0], rayList[1], rayList[2], 
                    rayList[3] - rayList[0], rayList[4] - rayList[1], rayList[5] - rayList[2], color=getColor(int(rayList[7]))) 
        if tag == 'norm':
            ax.quiver(rayList[0], rayList[1], rayList[2], 
                    0.3 * rayList[3], 0.3 * rayList[4], 0.3 * rayList[5], color=getColor(int(rayList[6]))) 
    
    # ax.scatter3D(278, 273, -800, color="red")  
    ax.grid()
    ax.set_xlabel('X')
    ax.set_xlim3d(-1, 1)
    ax.set_ylabel('Y')
    ax.set_ylim3d(-1, 1)
    ax.set_zlabel('Z')
    ax.set_zlim3d(-1, 1)
    plt.show()