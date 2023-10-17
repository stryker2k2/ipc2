CC= C:/cygwin64/bin/x86_64-w64-mingw32-gcc.exe
CC32 = C:/cygwin64/bin/i686-w64-mingw32-gcc.exe
MV = mv
RM = rm
MKDIR = mkdir
OUTPUT = ./output
BUILD = ./build

CFLAGS := -D__USE_MINGW_ANSI_STDIO=0 -w

all: payload client common_deps

# Payload
payload: $(BUILD)/payload.o
	$(CC) -shared $(BUILD)/mmHelper.o $(BUILD)/mmLoader.o $(BUILD)/payload.o -o $(OUTPUT)/$@.dll

$(BUILD)/payload.o: payload.c $(BUILD)/mmHelper.o $(BUILD)/mmLoader.o
	$(CC) -c $(CFLAGS) payload.c -o $@

# Client
client: $(BUILD)/client.o
	$(CC) $(BUILD)/mmHelper.o $(BUILD)/mmLoader.o $(BUILD)/client.o -o $(OUTPUT)/$@.exe

$(BUILD)/client.o: client.c $(BUILD)/mmHelper.o $(BUILD)/mmLoader.o
	$(CC) -c $(CFLAGS) client.c -o $@

# Dependencies
$(BUILD)/mmLoader.o:
	$(CC) -c $(CFLAGS) mmLoader.c -o $@

$(BUILD)/mmHelper.o:
	$(CC) -c $(CFLAGS) mmHelper.c -o $@

common_deps: $(BUILD)/common_deps.o
	$(CC) -shared common_deps.c -o $(OUTPUT)/$@.dll

$(BUILD)/common_deps.o: common_deps.c
	$(CC) -c $(CFLAGS) common_deps.c -o $@

# Misc
move:
	$(MKDIR) ./dll || TRUE
	$(RM) ./dll/payload.dll || TRUE
	$(MV) payload.dll ./dll/payload.dll

test-payload:
	rundll32.exe $(OUTPUT)/payload.dll,testMsgBox

test-client:
	$(OUTPUT)/client.exe

clean: 
	$(RM) $(OUTPUT)/*.* || TRUE
	$(RM) $(BUILD)/*.* || TRUE

./PHONY: all payload client common_deps clean