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
            
            if resp=="Arduino Ready":
                endFlag=True
                break

        if endFlag or (time.time()-lastReceivedTime>timeoutDuration):
            break

        time.sleep(0.01)


def OGfromArduino():
    receivedFlag=False
    start_time=time.time()
    timeout_duration=3
    while True:
        while ser.in_waiting:
            resp=ser.readline().decode()
            print(f"Arduino 回傳: {resp}")
            receivedFlag=True
        if (receivedFlag or (time.time()-start_time>timeout_duration)):
            break


def toArduino(reply):
    m=re.search(r"動作:\s*\[([A-Z0-9_,\s]+)\]", reply)
    if m:
        commands=[c.strip() for c in m.group(1).split(",")]
    else:
        commands=[]

    if len(commands)>4:
        commands=commands[-4:]

    payload=" ".join(commands) + "\n"
    ser.write(payload.encode())


def main():

    initArduino()

    firstRun=True

    with open("promptTC.txt", "r", encoding="utf-8") as f:
        instructions = f.read()

    while True:
        userInput=input("You: ").strip()
        if userInput.lower() in ("exit", "quit"):
            print("End session.")
            ser.close()             #############################################################################
            break

        prompt=userInput

        try:
            params = {
                "model": "gpt-4.1-mini",
                "instructions": instructions,
                "input": prompt
            }
            if not firstRun:
                params["previous_response_id"] = response.id
            response=client.responses.create(**params)
        except Exception as e:
            print(f"Errot: {e}")
            continue

        reply=response.output_text

        print(f"\nBot: {reply}\n")
        firstRun=False

        toArduino(reply)
        fromArduino()



if __name__=="__main__":
    main()









# import json
# from openai import OpenAI

# client=OpenAI()

# def main():
#     prompt=""
#     print("Start session, enter 'exit' to quit.\n")

#     while True:
#         user_input=input("You: ").strip()
#         if user_input.lower() in ("exit", "quit"):
#             print("End session.")
#             break

#         prompt+=f"User: {user_input}\nAssistant:"

#         try:
#             response=client.responses.create(
#                 model="gpt-4.1-mini",
#                 instructions="You are a helpful assistant.",
#                 input=prompt
                
#             )
#         except Exception as e:
#             print(f"Errot: {e}")
#             continue

#         reply=response.output_text

#         print(f"\nBot: {reply}\n")
#         prompt+=f"{reply}\n"

#         with open("chat_history.txt", "w", encoding="utf-8") as f:
#             f.write(prompt)

# if __name__=="__main__":
#     main()

