SFML_DIR = sfmlGraphicsPipeline
PROJECT_DIR = project
TARGET = ""

compile:
	make -j6 -C $(SFML_DIR)/build
	make -C $(PROJECT_DIR)/build $(TARGET)
	cd $(PROJECT_DIR)/build ; ./$(TARGET)

rebuild:
	mkdir -p $(SFML_DIR)/build
	cd $(SFML_DIR)/build ; cmake ..
	mkdir -p $(PROJECT_DIR)/build
	cd $(PROJECT_DIR)/build ; cmake ..