import cv2
import numpy
import sys

stdin = sys.stdin.buffer.read()
buffer = numpy.frombuffer(stdin, dtype='uint8')
img = cv2.imdecode(buffer, cv2.IMREAD_UNCHANGED)


def hex_to_bgra(value):
	"""Return [blue, green, red, alpha] for the color given as #rrggbb."""
	value = value.lstrip('#')
	lv = len(value)
	ret = list(int(value[i:i + lv // 3], 16) for i in range(lv-(lv // 3), -(lv // 3), -(lv // 3)))
	ret.append(255)
	return ret



#make a postcard out of the piped image
row, col = img.shape[:2]

babyblue = hex_to_bgra("#89cff0")
white = [255, 255, 255, 255]

bordersize = 30
img = cv2.copyMakeBorder(
	img,
	top=bordersize,
	bottom=bordersize,
	left=bordersize,
	right=bordersize,
	borderType=cv2.BORDER_CONSTANT,
	value=white
)
bordersize = 15
img = cv2.copyMakeBorder(
	img,
	top=bordersize,
	bottom=bordersize,
	left=bordersize,
	right=bordersize,
	borderType=cv2.BORDER_CONSTANT,
	value=babyblue
)
bordersize = 30
img = cv2.copyMakeBorder(
	img,
	top=bordersize,
	bottom=bordersize,
	left=bordersize,
	right=bordersize,
	borderType=cv2.BORDER_CONSTANT,
	value=white
)
cv2.imwrite("./test.png", img)
