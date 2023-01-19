import cv2
import numpy
import sys
import argparse

parser = argparse.ArgumentParser()

parser.add_argument("-s", "--styling", dest="styles", help="Styles to apply to the image", nargs="*")
parser.add_argument("-bc", "--border-color", dest="border-color", help="Optional RGB[A] hex code for border color in postcards", default="#89cff0", const="#89cff0", nargs="?")
parser.add_argument("-bg", "--background-color", dest="background-color", help="Optional RGB[A] hex code for the background color in postcards", default="#ffffff", const="#ffffff", nargs="?")

args = vars(parser.parse_args())

stdin = sys.stdin.buffer.read()
buffer = numpy.frombuffer(stdin, dtype='uint8')
img = cv2.imdecode(buffer, cv2.IMREAD_UNCHANGED)

row, col, channels = img.shape

if channels == 3:
	img = cv2.cvtColor(img, cv2.COLOR_BGR2BGRA)


def hex_to_bgra(value):
	"""Return [blue, green, red, alpha] for the color given as [#]rrggbb[aa]."""
	value = value.lstrip('#')
	lv = len(value)
	if lv != 6 and lv != 8:
		value = "000000"
	ret = [int(value[4:6], 16), int(value[2:4], 16), int(value[0:2], 16)]
	if len(value) == 8:
		ret.append(int(value[6:8], 16))
	else:
		ret.append(255)
	return ret


def get_optimal_font_scale(text, width, height, font, thickness):
	for scale in reversed(range(0, 60, 1)):
		textSize = cv2.getTextSize(text, fontFace=font, fontScale=scale/10, thickness=thickness)
		new_width = textSize[0][0]
		new_height = textSize[0][1]
		if (new_width <= width and new_height <= height):
			return scale/10
	return 1



#make a postcard out of the piped image
if args["styles"]:
	styles = args["styles"]
	if "postcard" in styles:
		bordercolor = hex_to_bgra(args["border-color"])
		background = hex_to_bgra(args["background-color"])

		bordersize = 30
		img = cv2.copyMakeBorder(
			img,
			top=bordersize,
			bottom=bordersize,
			left=bordersize,
			right=bordersize,
			borderType=cv2.BORDER_CONSTANT,
			value=background
		)
		bordersize = 15
		img = cv2.copyMakeBorder(
			img,
			top=bordersize,
			bottom=bordersize,
			left=bordersize,
			right=bordersize,
			borderType=cv2.BORDER_CONSTANT,
			value=bordercolor
		)
		bordersize = 30
		img = cv2.copyMakeBorder(
			img,
			top=bordersize,
			bottom=bordersize,
			left=bordersize,
			right=bordersize,
			borderType=cv2.BORDER_CONSTANT,
			value=background
		)

	if "banner" in styles:
		background = hex_to_bgra("#000000")
		bannerheight = 30
		img = cv2.copyMakeBorder(
			img,
			top=0,
			left=0,
			right=0,
			bottom=bannerheight,
			borderType=cv2.BORDER_CONSTANT,
			value=background
		)

		endwidth = col + (150 if "postcard" in styles else 0)
		font = cv2.FONT_ITALIC
		message = "Project webserv banner!"
		thickness = 2
		scale = get_optimal_font_scale(message, endwidth, bannerheight - 10, font, thickness)
		textSize = cv2.getTextSize(message, fontFace=font, fontScale=scale, thickness=thickness)[0]
		img = cv2.putText(img, message,
			((endwidth // 2) - (textSize[0] // 2), row + bannerheight - 5 + (150 if "postcard" in styles else 0)),
			font,
			scale,
			hex_to_bgra("#FFFFFF"),
			thickness,
			cv2.LINE_AA)

cv2.imwrite("./test.png", img)
