import subprocess
import random
import sys
import os

def random_int():
	sign = random.randint(0,1)
	if sign:
		return "_"+str(random.randint(1,1000))
	else:
		return str(random.randint(1,1000))

def anomaly():
	return os.urandom(16)

def random_operator():
	operators = ["+","-","*","%","/","^"]
	return random.choice(operators)

def random_command():
	commands = ["p","q","f","r","d","c"]
	return random.choice(commands)


def generate_testcase(numlines):

	funcs = [random_int, random_operator, random_command]

	out = ""
	for i in range(numlines):
		out += (random.choice(funcs)() + "\n")
	return out

def get_testcase_output(exe_name,inpt):
	p = subprocess.Popen(exe_name,stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
	p.stdin.write(inpt)
	p.stdin.close()
	out = p.stdout.read(30).strip()
	err = p.stderr.read(100).strip()
	return out, err

def make_dumpfile(content):
	global numdumps
	f = open("failure_dumps/dump%d"%numdumps,"wb")
	f.write(content)
	f.close()
	numdumps += 1


if __name__ == "__main__":

	global numdumps
	numdumps = 0

	if (len(sys.argv) !=2 ):
		print("usage: ./fuzz.py [NUMTESTS]")
		exit(-1)

	os.system("mkdir -p failure_dumps")
	NUMTESTS = int(sys.argv[1])
	CHSET = ["/","*"," ","\n","a","/*","*/"]
	num = 0

	for i in range(NUMTESTS):
		fcontent = generate_testcase (random.randint(1,10))
		out1, err1 = get_testcase_output("dc",fcontent)
		out2, err2 = get_testcase_output("./mydc",fcontent)

		if(out1 != out2 or err1 != err2):

			print("[-] testcase%d failed"%i)
			make_dumpfile (fcontent)
		else:
			print("[+] testcase%d successful"%i)
			num+=1

	print("[*] passed %d/%d tests"%(num,NUMTESTS))


