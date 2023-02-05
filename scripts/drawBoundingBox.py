import matplotlib.pyplot as plt
import numpy as np

def drawSphere(ax, R, x, y, z):
    u = np.linspace(0, 2 * np.pi, 100)# 用参数方程画图
    v = np.linspace(0, np.pi, 100)
    x = R * np.outer(np.cos(u), np.sin(v)) + x# outer()外积函数：返回cosu × sinv
    y = R * np.outer(np.sin(u), np.sin(v)) + y# 
    z = R * np.outer(np.ones(np.size(u)), np.cos(v)) + z# ones()：返回一组[1,1,.......]

    ax.plot_surface(x, y, z, alpha=0.5)

edgeIndex = [[0, 1, 3, 2, 0], [4, 5, 7, 6, 4], [1, 5, 4, 0, 1], [2, 6, 7, 3, 2]]
pointindex = [[0, 0, 0], [0, 0, 1], [0, 1, 0], [0, 1, 1], [1, 0, 0], [1, 0, 1], [1, 1, 0], [1, 1, 1]]
XInd = 0
YInd = 1
ZInd = 2

myColor = ['red', 'blue', 'green', 'cyan', 'orange', 'yellow']
colorInd = 0
def getColor():
    global colorInd
    if colorInd < len(myColor) - 1:
        colorInd += 1
    else:
        colorInd = 0
    return myColor[colorInd]


if __name__ == '__main__':

    fig=plt.figure()
    ax=fig.add_subplot(projection='3d')
    f = open("./Boxes04.txt")
    lines = f.readlines()
    for line in lines:
        rayList = [float(x) for x in line.split()]
        if len(rayList) == 6:
            boxRange = [[rayList[0], rayList[1], rayList[2]],[rayList[3], rayList[4], rayList[5]]]
            boundColor = getColor()
            points = [[0, 0, 0] for i in range(8)]
            for i in range(8) :
                xChoose = pointindex[i][0]
                yChoose = pointindex[i][1]
                zChoose = pointindex[i][2]
                points[i] = [boxRange[xChoose][XInd], boxRange[yChoose][YInd], boxRange[zChoose][ZInd]]
                # ax.scatter3D(boxRange[xChoose][XInd], boxRange[yChoose][YInd], boxRange[zChoose][ZInd], color=boundColor)  
            for edgeStripInd in edgeIndex:
                # edgeStripInd = [0, 1, 3, 2]
                xx = [points[pointInd][XInd] for pointInd in edgeStripInd]
                yy = [points[pointInd][YInd] for pointInd in edgeStripInd]
                zz = [points[pointInd][ZInd] for pointInd in edgeStripInd]
                ax.plot(xx, yy, zz, color=boundColor)


    
    # ax.scatter3D(0, 0, -0, color="red")  
    ax.grid()
    ax.set_xlabel('X')
    ax.set_xlim3d(-2, 2)
    ax.set_ylabel('Y')
    ax.set_ylim3d(-2, 2)
    ax.set_zlabel('Z')
    ax.set_zlim3d(-2, 2)
    plt.show()