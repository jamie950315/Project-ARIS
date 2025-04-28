import json
from openai import OpenAI
import serial
import time
import re

client=OpenAI()


def initArduino():
    COM_PORT="COM5"
    BAUD_RATES=115200
    global ser
    ser=serial.Serial(COM_PORT, BAUD_RATES, timeout=0.5)
    time.sleep(0.1)
    fromArduino()

def fromArduino():
    endFlag=False
    lastReceivedTime=time.time()
    timeoutDuration=3

    while True:
        while ser.in_waiting:
            resp=ser.readline().decode(errors='ignore').strip()
            print(f"Arduino: {resp}")
            lastReceivedTime=time.time()
            
            if(resp=="Arduino Ready"):
                endFlag=True
                break

        if(endFlag or (time.time()-lastReceivedTime>timeoutDuration)):
            break

        time.sleep(0.01)


def toArduino(reply):
    m=re.search(r"動作:\s*\[([A-Z0-9_,\s]+)\]", reply)
    if m:
        commands=[c.strip() for c in m.group(1).split(",")]
    else:
        commands=[]

    if(len(commands)>4):
        commands=commands[-4:]

    payload=" ".join(commands)+"\n"
    ser.write(payload.encode())


def main():

    initArduino()

    firstRun=True

    with open("promptTC.txt", "r", encoding="utf-8") as f:
        instructions = f.read()

    while True:
        userInput=input("You: ").strip()
        if(userInput.lower() in ("exit", "quit")):
            print("End session.")
            ser.close()
            break

        prompt=userInput

        try:
            params={
                "model": "gpt-4.1-mini",
                "instructions": instructions,
                "input": prompt
            }
            if(not firstRun):
                params["previous_response_id"] = response.id
            response=client.responses.create(**params)
        except Exception as e:
            print(f"Error: {e}")
            continue

        reply=response.output_text

        print(f"\nBot: {reply}\n")
        firstRun=False

        toArduino(reply)
        fromArduino()



if __name__=="__main__":
    main()

