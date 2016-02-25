#!/usr/bin/python
# -*- coding: iso-8859-15 -*-

import urllib.request
import re
import os, sys
import getopt
import random

__trainingPath__ = "./" 
__imagePath__    = __trainingPath__ + "images/"

def main():
	
	if len(sys.argv) > 1:
		if sys.argv[1] == '-v': #Filters away entries that don't have enough data. Reads input and writes to output stream.
			print (filterValid(sys.stdin))
		elif sys.argv[1] == '-u': #Filers away products with same image as another. Reads input and writes to output stream.
			print(filterUnique(sys.stdin))
		elif sys.argv[1] == '-t': #Reserved for development tests.
			tests()
		elif sys.argv[1] == '--combine': #Concats the data of file1 to the end of file2. Needs path to file1 and path2.
			combine()
		elif sys.argv[1] == '--shuffle': #Shuffles inputed data. Reads input and writes to output stream.
			print(shuffle(sys.stdin))
		elif sys.argv[1] == '--makeReady': #Returns only the data needed for the machine training. Reads input and writes to output stream.
			for line in makeReady(sys.stdin):
				print(line)
		elif sys.argv[1] == '-d': #Downloads the small images to "./images/". Reads input stream.
			downloadImages()
		elif sys.argv[1] == '-c':
			print("WIP")
		elif sys.argv[1] == '-a': #Do every needed step to make raw .csv to readable file.
			print(makeReady(shuffle(filterUnique(filterValid(sys.stdin)))))
		elif sys.argv[1] == '-x': #Do every needed step before makeReady.
			for line in (shuffle(filterUnique(filterValid(sys.stdin)))):
				print(line)
		elif sys.argv[1] == '--fc': #Counts the amount of sleeve types, 0 unknown, 1 sleevless, 2 short, 3 long. Reads from input and writes to output stream.
			featCheck = [0,0,0,0];
			for line in sys.stdin:
				featCheck[parseDetails(getDetails(line))[0]+1] += 1
			print (featCheck)
		elif sys.argv[1] == '--fixMissingType':
			fixOrder()
			
	else:
		lines = "20 Ak\n30 Mo\n10 Ti\n"
		lines2 = "6632411;ProductImages/6632/6632308/S0000006632407_F_W30_20110623102901.jpg;ProductImages/6632/6632308/S0000006632407_F_W40_20110623102904.jpg;Fitted soft washed denim shirt with chest pockets, classic collar and snap closure. Rounded hemline. Length 70 cm in size 38.;NULL;Bl�"
		lines3 = "6853721;ProductImages/6843/6843006/S0000006853723_F_W30_20120531113439.jpg;ProductImages/6843/6843006/S0000006853723_F_W40_20120531113447.jpg;Shiny viscose blouse with a chiffon frill at the bottom. Short cap sleeves and round neckline with an opening at the neck. Small decorative jewelry at front. Length 73 cm in size M.;NULL;R�d"
		lines4 = "6874431;ProductImages/6862/6862149/S0000006874429_F_W30_20120821155014.jpg;ProductImages/6862/6862149/S0000006874429_F_W40_20120821155022.jpg;Long shirt with a shiny fabric at the front, back and at the sleeves. Transparent chiffon at the top, bottom and sleeves. Classic collar with stud detailing. Chest pockets and concealed buttoning at front. Slit opening at the bottom sides and longer back. Length 85 cm in size 38.;NULL;Svart"
		lines5 = "6874814;ProductImages/6850/6850378/S0000006874811_F_W30_20120808140450.jpg;ProductImages/6850/6850378/S0000006874811_F_W40_20120808140457.jpg;Top with embellishment at front. Round neckline with an opening at the neck and button closure. Short chiffon sleeves. Length 64 cm in size 38.;NULL;Svart"
		lines6 = "6760826;Blus;Blouse;ProductImages/6715/6715702/S0000006760827_F_W30_20120103105708.jpg;ProductImages/6715/6715702/S0000006760827_F_W40_20120103105716.jpg;Patterned sleeveless blouse in a soft and silky fabric. V-shaped placket at front with two hidden snaps. Counter-pleat at back with a rounded hemline. Length 65 cm in size 38.;NULL;Orange"
		
		print (getProductId(lines2))
		print (getSmallImageLoc(lines2))
		print (getBigImageLoc(lines2))
		print (getColor(lines2))
		print (getDetails(lines2))
		
		print (parseDetails(getDetails(lines2)))
		print (parseDetails(getDetails(lines3)))
		print (parseDetails(getDetails(lines4)))
		print (parseDetails(getDetails(lines5)))

def tests():
	s  = set()
	c  = set()
	sl = set()
	
	
	
	nameList = []
	countList = []
	
	for line in sys.stdin:
		clothType = getTypeEng(line)
		if(clothType == "Playsuit"):
			print(getBigImageLoc(line))
		
		color     = getColor(line)
		sleeve    = parseDetails(getDetails(line))
		if clothType not in s:
			s.add(clothType)
			nameList.append(clothType)
			countList.append(1)
		else:
			for index in range(len(nameList)):
				if nameList[index] == clothType:
					countList[index] = countList[index] + 1
		
		if color not in c:
			c.add(color)
			print(color)
			
		if sleeve not in sl:
			sl.add(sleeve)
	
	for index in range(len(nameList)):
		print (nameList[index] + ": " + str(countList[index]))
	
	print (sorted(c))
	print (sorted(s))
	print (sorted(sl))
	print("kov")
	
def fixOrder():
	pattern  = re.compile('[0-9]+;')
	
	#line = "6775122;ProductImages/6762/6762106/S0000006775131_F_W30_20120719124606.jpg;ProductImages/6762/6762106/S0000006775131_F_W40_20120719124610.jpg;Five-pocket style stretch jeans with regular waist. Fitted through the thigh and straight leg from the knee down. Inside leg measures 82 cm in length 32. ;NULL;Bl�"
	
	for line in filterValid2(sys.stdin):
		if line != '\n':
			line = line.replace('\n','')
			found  = pattern.search(line)
			
			middle = "skirt;"
					
			first = line[0:found.end()]
			last = line[found.end():len(line)]
			
			print (first+middle+middle+last)
	
	
	
def fixOrderByxor():
	re.UNICODE
	pattern = re.compile('[0-9]+;')
	
	patternLeggings = re.compile('(legging|Legging|jegging|treggings)+')
	patternTrousers = re.compile('(trouser|Trouser|trousres|pants)+')
	patternShorts   = re.compile('(shorts|Shorts)+')
	patternChinos   = re.compile('(chinos|Chinos)+')
	
	
	for line in filterValid2(sys.stdin):
		if line != '\n':
			found = pattern.search(line)
			
			middle = ";"
			
			if(patternTrousers.search(line)):
				middle = "trousers;"
			if(patternShorts.search(line)):
				middle = "shorts;"
			if(patternLeggings.search(line)):
				middle = "leggings;"
			if(patternChinos.search(line)):
				middle = "chinos;"
				
			line = line.replace('\n','')
			first = line[0:found.end()]
			last = line[found.end():len(line)]
			
			print (first+middle+middle+last)
		
def filterValid2(input):
	re.UNICODE
	pattern = re.compile('[0-9]+;([A-z0-9\ \,\.\/]+)(.jpg|.png);([A-z0-9\ \,\.\/]+)(.jpg|.png);([A-z0-9\ \,\.\-\!]+);[A-z0-9\ \,\.\-\!]+;[A-�a-�]+')
	
	out = []
	for line in input:
		if pattern.match(line):
			line = line.replace('\n','')
			out.append(line)
	return out
	
def shuffle(input):
	out = []
	tmp = []

	for line in input:
		if(line != ""):
			tmp.append(line)
	
	random.shuffle(tmp)
	
	for line in tmp:
		line = line.replace('\n','')
		out.append(line)
	return out
	
	
def combine():
	filepath1 = sys.argv[2]
	filepath2 = sys.argv[3]
	
	file1 = open(filepath1, 'r')
	file2 = open(filepath2, 'a')
	
	tmp = file1.read()
	file2.write(tmp)
	
	file1.close()
	file2.close()
	
	
def makeReady(input):
	out = []
	for line in input:
		if(getTypeEng(line) != "Bangle"):
			id         = getProductId(line)
			path       = downloadImage(line)
			color      = getColor(line)
			clothType  = getTypeEng(line)
			sleeveType = parseDetails(getDetails(line))

			header = ('#' + str(len(id)) + ';' + str(len(path)) + ';' + str(len(color)) + 
					  ';' + str(len(clothType)) + ';' + str(len(sleeveType)) + '#')
			
			output = header + id + path + color + clothType + sleeveType
			out.append(output)
	return out
				 
		
		
		
	
def downloadImage(input):
	id       = getProductId(input)
	partAddr = getSmallImageLoc(input)
	fileExtd = pngOrjpg(partAddr)
	addr     = "http://static.lindex.com/Archive/" + partAddr
	filePath  = __imagePath__ + id + fileExtd
	urllib.request.urlretrieve(addr , filePath)
	return filePath
	
def downloadImages():
	for line in sys.stdin:
		downloadImage(line)
	
def pngOrjpg(input):
	re.UNICODE
	pattern = re.compile('(jpg)')
	if pattern.search(input):
		return ".jpg"
	else:
		return ".png"
	
def filterValid(input):
	re.UNICODE
	pattern = re.compile('[0-9]+;[A-�\ \,\.\-\!]+;[A-�\ \,\.\-\!]+;([A-z0-9\ \,\.\/]+)(.jpg|.png);([A-z0-9\ \,\.\/]+)(.jpg|.png);([A-z0-9\ \,\.\-\!]+);[A-z0-9\ \,\.\-\!]+;[A-�a-�]+')
	
	out = []
	for line in input:
		# regexp som kollar att all nodvandig info finns
		# lagg rader som ar godkanda i lines2
		if pattern.match(line):
			line = line.replace('\n','')
			out.append(line)
	return out
	
def filterUnique(input): # Funkar inte helt, finns dubbletter som inte ligger direkt efter varandra.
	unique = set()
	out = []
	for line in input:
		smallImgLoc = getSmallImageLoc(line)
		if smallImgLoc not in unique:
			unique.add(smallImgLoc)
			line = line.replace('\n','')
			out.append(line)
	return out
	

def getProductId(input):
	re.UNICODE
	pattern = re.compile('[0-9]+')
	found   = pattern.search(input)
	
	if found:
		return(input[:found.end()])
			
def getColor(input):
	re.UNICODE
	pattern = re.compile('[A-z]+$')
	found   = pattern.search(input)
	
	if found:
		tmp = input[found.start():len(input)]
		tmp = tmp.replace('\n','')
		if(tmp == "d" or tmp == "n"):
			print (input)
		return(tmp)

def getSmallImageLoc(input):
	re.UNICODE
	patternFirst  = re.compile('[0-9]+;[A-�\ \,\.\-\!]+;[A-�\ \,\.\-\!]+;')
	patternSecond = re.compile('[0-9]+;[A-�\ \,\.\-\!]+;[A-�\ \,\.\-\!]+;[A-z0-9/]+(.jpg|.png)')
	
	foundFirst  = patternFirst.search(input)
	foundSecond = patternSecond.search(input)
	
	if foundFirst and foundSecond:
		posFirst  = foundFirst.end()
		posSecond = foundSecond.end()
		return (input[posFirst:posSecond])

def getBigImageLoc(input):
	re.UNICODE
	patternFirst  = re.compile('[0-9]+;[A-�\ \,\.\-\!]+;[A-�\ \,\.\-\!]+;[A-z0-9/]+(.jpg|.png);')
	patternSecond = re.compile('[0-9]+;[A-�\ \,\.\-\!]+;[A-�\ \,\.\-\!]+;[A-z0-9/]+(.jpg|.png);[A-z0-9/]+(.jpg|.png)')
	
	foundFirst  = patternFirst.search(input)
	foundSecond = patternSecond.search(input)
	
	if foundFirst and foundSecond:
		posFirst  = foundFirst.end()
		posSecond = foundSecond.end()
		return (input[posFirst:posSecond])
	
def getTypeSwe(input):
	re.UNICODE
	patternFirst  = re.compile('[0-9]+;')
	patternSecond = re.compile('[0-9]+;[A-�\ \,\.\-\!]+')
	
	foundFirst  = patternFirst.search(input)
	foundSecond = patternSecond.search(input)
	
	if foundFirst and foundSecond:
		posFirst  = foundFirst.end()
		posSecond = foundSecond.end()
		return (input[posFirst:posSecond])
	

def getTypeEng(input):     # <--- Stoppa in en massa nya patterns...
	input = input.lower()
	patternFirst  = re.compile('[0-9]+;[A-�\ \,\.\-\!]+;')
	patternSecond = re.compile('[0-9]+;[A-�\ \,\.\-\!]+;[A-z\ \,\.\-\!]+')
	
	foundFirst  = patternFirst.search(input)
	foundSecond = patternSecond.search(input)
	
	if foundFirst and foundSecond:
		posFirst  = foundFirst.end()
		posSecond = foundSecond.end()
		input = input[posFirst:posSecond]
			
		patternTanktop    = re.compile('(tank top|tanktop)')
		patternTop        = re.compile('(top|topp)')
		patternBangle     = re.compile('(bangle)')
		patternBlazer     = re.compile('(blazer)')
		patternBlouse     = re.compile('(blouse)')
		patternCamisole   = re.compile('(camisole)')
		patternKaftan     = re.compile('(kaftan)')
		patternTunic      = re.compile('(tunic)')
		patternSinglet    = re.compile('(singlet)')
		patternTShirt     = re.compile('(t-shirt)')
		patternSweatshirt = re.compile('(sweater)')
		patternShirt      = re.compile('(shirt)')
		patternSkirt      = re.compile('(skirt)')
		patternPlaysuit   = re.compile('(playsuit)')
		patternJumpsuit   = re.compile('(jumpsuit)')
		patternJacket     = re.compile('(jacket)')
		patternDress      = re.compile('(dress)')
		patternKimono     = re.compile('(kimono)')
		patternKl         = re.compile('(kl)')
		
		patternJeans      = re.compile('(jeans)')
		patternTights     = re.compile('(tights)')
		patternTrousers   = re.compile('(trousers)')
		patternLeggings   = re.compile('(leggings)')
		patternChinos     = re.compile('(chinos)')
		patternShorts     = re.compile('(shorts)')
		
		if(patternTShirt.search(input)):
			return ("Tshirt")
		if(patternTanktop.search(input)):
			return ("Tanktop")
		if(patternTop.search(input)):
			return ("Top")
		if(patternBangle.search(input)):
			return ("Bangle")
		if(patternBlazer.search(input)):
			return ("Blazer")
		if(patternBlouse.search(input)):
			return ("Blouse")
		if(patternCamisole.search(input)):
			return ("Camisole")
		if(patternTunic.search(input)):
			return ("Tunic")
		if(patternKaftan.search(input)):
			return ("Tunic")
		if(patternSinglet.search(input)):
			return ("Singlet")
		if(patternSweatshirt.search(input)):
			return ("Sweatshirt")
		if(patternShirt.search(input)):
			return ("Shirt")
		if(patternSkirt.search(input)):
			return ("Skirt")
		if(patternPlaysuit.search(input)):
			return ("Playsuit")
		if(patternJumpsuit.search(input)):
			return ("Jumpsuit")
		if(patternJacket.search(input)):
			return ("Jacket")
		if(patternDress.search(input)):
			return ("Dress")
		if(patternKimono.search(input)):
			return ("Kimono")
		if(patternKl.search(input)):
			return ("Dress")
		
		if(patternJeans.search(input)):
			return ("Jeans")
		if(patternTights.search(input)):
			return ("Tights")
		if(patternTrousers.search(input)):
			return ("Trousers")
		if(patternLeggings.search(input)):
			return ("Leggings")
		if(patternChinos.search(input)):
			return ("Chinos")
		if(patternShorts.search(input)):
			return ("Shorts")
		
		print(input)
		return ("WRONG")
	
def getDetails(input):
	re.UNICODE
	patternFirst  = re.compile('[A-�0-9\.\ \,\-\!]+;[A-�0-9\.\ \,\-\!]+;[A-�]+$')
	patternSecond = re.compile(';[A-�0-9\.\ \,\-\!]+;[A-�]+$')
	
	foundFirst  = patternFirst.search(input)
	foundSecond = patternSecond.search(input)
	
	if foundFirst and foundSecond:
		posFirst  = foundFirst.start()
		posSecond = foundSecond.start()
		return (input[posFirst:posSecond])
		
		
def parseDetails(input): #0 no sleeves, 1 short sleeves, 2 long sleeves, -1 uknown.
	input = input.lower()
	
	patternSleeveless   = re.compile('(sleeveless)')
	patternShortSleeves = re.compile('(short)[A-z\ ]*(sleeves)')
	patternLongSleeves  = re.compile('(long)[A-z\ ]*(sleeves)')
	
	foundSleeveless   = patternSleeveless.search(input)
	foundShortSleeves = patternShortSleeves.search(input)
	foundLongSleeves  = patternLongSleeves.search(input)
	
	if foundSleeveless:
		return "0"
	if foundShortSleeves:
		return "1"
	if foundLongSleeves:
		return "2"
	
	return "-1"
	
#Long / Short sleaves
#Colors
#Patterns
#Type of clothing

	
if __name__ == "__main__":
    main()
