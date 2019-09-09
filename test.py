count = 0
def find_value(x):
    bin_count = bin(x) # Convert decimal value to binary value
    # Convert binary value to string
    if x<2:
        string_bin = "00"+str(bin_count)[2:] #Append 00 to front if number less than 2
    elif x<4:
        string_bin = "0"+str(bin_count)[2:] #Append 0 to front if number less than 4
    else:
        string_bin = str(bin_count)[2:] 
    #Set each GPIO with corresponding binary value
    #List indexed and strings converted to ints
    print(int(string_bin[0:1]), end="")
    print(int(string_bin[1:2]), end="")
    print(int(string_bin[2:]), end="")
    return

def main():
    global count
    if count < 7:
        count += 1
        v = print("run once")  
        find_value(count)
    else:
        exit()

if __name__ == "__main__":
    while True:
        main()