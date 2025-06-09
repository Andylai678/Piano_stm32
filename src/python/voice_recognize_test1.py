import whisper
import sounddevice as sd
from scipy.io.wavfile import write
import serial
import threading

SERIAL_PORT = 'COM3'
BAUD_RATE = 115200
AUDIO_FILE = "./recorded.wav"
SAMPLE_RATE = 16000
DURATION = 5

model = whisper.load_model("base")
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

song_names = [
    "let it be", "imagine", "yesterday", "hey jude", "hello",
    "shape of you", "perfect", "believer", "counting stars", "faded"
]

def normalize_text(text):
    return text.strip().lower()

def find_song_index(recognized_text):
    text = normalize_text(recognized_text)
    for idx, name in enumerate(song_names):
        if name in text:
            return idx
    return -1

def record_audio():
    print("Recording...")
    audio = sd.rec(int(DURATION * SAMPLE_RATE),
                   samplerate=SAMPLE_RATE,
                   channels=1,
                   blocking=True)
    write(AUDIO_FILE, SAMPLE_RATE, audio)
    print("Recording complete")
    return audio

def process_audio():
    print("Analyzing...")
    result = model.transcribe(AUDIO_FILE)
    text = result["text"]
    print(f"Analyze Result：{text}")

    song_index = find_song_index(text)
    if song_index >= 0:
        song_name = song_names[song_index]
        print(f"Sending Song name：{song_name}")
        padded = song_name[:15].ljust(15) + '\0'  # LCD 限制長度 + 結尾符號
        ser.write(padded.encode('utf-8'))
    else:
        error_msg = "Not found".ljust(15) + '\0'
        print("Unknown song, sending error message")
        ser.write(error_msg.encode('utf-8'))

def serial_listener():
    while True:
        if ser.in_waiting:
            line = ser.readline().decode().strip()
            if line == "START_RECORDING":
                print("\nReceive STM32 instr：START_RECORDING")
                record_audio()
                process_audio()

if __name__ == "__main__":
    print("Start to Receive STM32 instr...")
    listener_thread = threading.Thread(target=serial_listener)
    listener_thread.daemon = True
    listener_thread.start()

    try:
        while True:
            pass
    except KeyboardInterrupt:
        print("\n👋 離開程式")
        ser.close()
