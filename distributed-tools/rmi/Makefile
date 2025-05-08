FILES := $(wildcard *.java)

.PHONY: clean run compile run-client

compile: $(FILES)
	@echo "Compiling Java files..."
	@javac $(FILES)
	@echo "Compilation complete."

clean:
	@echo "Cleaning up..."
	@rm -f *.class
	@echo "Clean up complete."

run: $(FILES)
	@echo "Running Java program..."
	@java Main
	@echo "Execution complete."

run-client: $(FILES)
	@echo "Running server..."
	@java Client
	@echo "Server execution complete."
