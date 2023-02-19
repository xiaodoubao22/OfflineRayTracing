import matplotlib.pyplot as plt
import random

class Sobel:
    a = []
    m = []
    sj = []
    v_ = []
    idxOffs = 1
    i = 0
    xlast = 0

    def __init__(self, a, m):
        self.a = a
        self.m = m
        self.sj = len(m)
        self.v_ = []
        # 生成m
        for k in range(self.sj + 1, 33):  # k~[1,32]
            mk = self.m[k - self.sj - self.idxOffs]
            mk = mk ^ ((2 ** self.sj) * self.m[k - self.sj - self.idxOffs])
            for i in range(1, self.sj):  # i~[1, sj-1]
                mk = mk ^ ((2 ** i) * self.m[k - i - self.idxOffs] * a[i - self.idxOffs]) 
            self.m.append(mk)
        

        # 生成v
        for k in range(1, 33):  # k~[1,32]
            vk = self.m[k - self.idxOffs] << (32 - k)
            self.v_.append(vk)
        print(self.v_)
        return

        


    def Generate(self):
        # print(self.i)
        i = self.i
        self.i += 1

        ci = 1
        while (i & 1) != 0:
            i = i >> 1
            ci += 1
        x = self.xlast ^ self.v_[ci - 1]
        self.xlast = x
        return x / (1 << 32)

ax = [0, 1]
mx = [1, 3, 7]
ay = [1, 0]
my = [1, 1, 1]

sobelx = Sobel(ax, mx)
sobely = Sobel(ay, my)

for i in range(32):
    x1 = sobelx.Generate()
    # y1 = sobely.Generate()


# fig=plt.figure(figsize=(12,6))
# ax1=fig.add_subplot(1, 2, 1)
# ax1.set_xlabel('X')
# ax1.set_xlim(0, 1)
# ax1.set_ylabel('Y')
# ax1.set_ylim(0, 1)
# ax2=fig.add_subplot(1, 2, 2)
# ax2.set_xlabel('X')
# ax2.set_xlim(0, 1)
# ax2.set_ylabel('Y')
# ax2.set_ylim(0, 1)

# plt.ion()
# for i in range(100): # i~[0,9]
#     x1 = sobelx.Generate()
#     y1 = sobely.Generate()
#     x2 = random.random()
#     y2 = random.random()
#     points1 = ax1.scatter([x1], [y1], color=(0, 0.6, 0.9, 1))
#     points2 = ax2.scatter([x2], [y2], color=(0, 0.6, 0.9, 1))
#     plt.draw()
#     plt.pause(0.01)
# plt.ioff()
# plt.show()



