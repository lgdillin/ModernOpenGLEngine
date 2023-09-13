import cv2 as cv
import numpy as np

img = cv.imread('rusty.png', cv.IMREAD_COLOR)
# img = cv.cvtColor(img, cv.COLOR_RGBA2BGRA)
# print(img.shape)

b,g,r = cv.split(img)
# or use this method
# b = img[:,:,0]
#r = img[0,0,:]
#r = cv.merge([r,r,r])

# b = cv.bitwise_not(b)
# g = cv.bitwise_not(g)
# r = cv.bitwise_not(r)

# img2 = cv.bitwise_not(b)
img2 = g

ret,level1 = cv.threshold(img2, 127, 255, cv.THRESH_TOZERO)
ret,level2 = cv.threshold(img2, 160, 255, cv.THRESH_TOZERO)
ret,level3 = cv.threshold(img2, 180, 255, cv.THRESH_TOZERO)

#level2 = cv.cvtColor(level2, cv.COLOR_GRAY2BGRA)

#g = cv.bitwise_not(g)
#cv.imwrite('grass_specular.png', level1)
# level4 = level1 * 1.1
#level4 = cv.addWeighted(level1, 0.5, level3, 0.5, 0)
cv.imwrite('rusty_specular.png', level1)

# cv.imshow('level1', level1)
# cv.imshow('level2', level2)
# cv.imshow('level3', level3)

# cv.imshow('b', b)
# cv.imshow('g', g)
# cv.imshow('r', r)
# k = cv.waitKey(0)