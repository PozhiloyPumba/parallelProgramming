import numpy as np
import matplotlib.pyplot as plt

names = ['def', 'stat', 'dyn', 'guid']
labels = [name + i for name in names for i in ['1', '4']]

colors = ['red', 'blue', 'orange', 'green']

allList = []
for i in range(len(labels)):
    allList.append([list(map(int, input().split())) for i in range(65)])
    allList[-1].sort(key=lambda a: a[1])

width = 0.8

fig, (ax, ax_table) = plt.subplots(nrows=2, gridspec_kw=dict(height_ratios=[3,1]))

ax_table.axis('off')

addedLabels = []
for i in range(len(allList[0])):
    curLevel = [allList[j][i] for j in range(len(labels))]
    curColors = [colors[curLevel[j][0]] for j in range(len(labels))]
    ax.bar(labels, 1, width, bottom=i, color=curColors, label='Thread: ' + str(curLevel[0][0]) if curLevel[0][0] not in addedLabels else '')

    addedLabels.append(curLevel[0][0])
ax.set_ylabel('num iteration')
ax.set_xlabel('shedule type')
ax.set_title('distribution')
ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.3),
          fancybox=True, shadow=True, ncol=5) 
ax.tick_params(axis='x', labelrotation=60)

plt.subplots_adjust(bottom=0.05)
plt.show()