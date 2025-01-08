CXXFLAGS ?=-Wall -Wpedantic -Wextra
PROTOC ?= $(shell which protoc)
# PKG_CONGFIG ?= $(shell which pkg-config)
BUILD_DIR ?= out
GRPC_SRC=${BUILD_DIR}/dispatcher.pb.cc ${BUILD_DIR}/dispatcher.grpc.pb.cc
GRPC_OBJ=$(GRPC_SRC:.cc=.o)
LDLIBS=-lfcgi -lpthread -lgrpc++_reflection

PROTOBUF_ABSL_DEPS=absl_absl_check absl_absl_log absl_algorithm absl_base absl_bind_front \
absl_bits absl_btree absl_cleanup absl_cord absl_core_headers absl_debugging absl_die_if_null \
absl_dynamic_annotations absl_flags absl_flat_hash_map absl_flat_hash_set absl_function_ref \
absl_hash absl_layout absl_log_initialize absl_log_severity absl_memory absl_node_hash_map \
absl_node_hash_set absl_optional absl_span absl_status absl_statusor absl_strings \
absl_synchronization absl_time absl_type_traits absl_utility absl_variant
LDLIBS+=$(shell pkg-config --libs --static protobuf grpc++ absl_flags absl_flags_parse $(PROTOBUF_ABSL_DEPS))

CXXFLAGS += -I$(BUILD_DIR)

$(BUILD_DIR)/%.pb.cc: %.proto
	mkdir -p $(dir $@)
	$(PROTOC) --grpc_out=out --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` --cpp_out=out $<

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

dispatcher: $(BUILD_DIR)/dispatcher.pb.cc ${GRPC_OBJ} $(BUILD_DIR)/dispatcher.o
	@echo PKG_PROG_PKG_CONFIG=$(PKG_PROG_PKG_CONFIG)
	@echo LDFLAGS=$(LDFLAGS)
	@echo LDLIBS=$(LDLIBS)
	$(CXX) $(LDFLAGS) ${GRPC_OBJ} $(BUILD_DIR)/dispatcher.o -o dispatcher $(LDLIBS)

.PHONY:
clean:
	rm -rf $(BUILD_DIR)
