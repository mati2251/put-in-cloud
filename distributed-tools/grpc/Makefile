all: server client

# Arch pkg-config is invalid
CC_FLAGS = -DPROTOBUF_USE_DLLS -DNOMINMAX -DOPENSSL_LOAD_CONF -pthread -DNOMINMAX -lprotobuf -labsl_log_internal_check_op -labsl_leak_check -labsl_die_if_null -labsl_log_internal_conditions -labsl_log_internal_message -labsl_examine_stack -labsl_log_internal_format -labsl_log_internal_proto -labsl_log_internal_nullguard -labsl_log_internal_log_sink_set -labsl_log_sink -labsl_log_entry -labsl_log_initialize -labsl_log_globals -labsl_vlog_config_internal -labsl_log_internal_fnmatch -labsl_log_internal_globals -lutf8_validity -lutf8_range -lgrpc -labsl_statusor -lgpr -labsl_flags_internal -labsl_flags_reflection -labsl_flags_private_handle_accessor -labsl_flags_commandlineflag -labsl_flags_commandlineflag_internal -labsl_flags_config -labsl_flags_program_name -labsl_raw_hash_set -labsl_hash -labsl_city -labsl_low_level_hash -labsl_hashtablez_sampler -labsl_flags_marshalling -labsl_random_distributions -labsl_random_seed_sequences -labsl_random_internal_pool_urbg -labsl_random_internal_randen -labsl_random_internal_randen_hwaes -labsl_random_internal_randen_hwaes_impl -labsl_random_internal_randen_slow -labsl_random_internal_platform -labsl_random_internal_seed_material -labsl_random_seed_gen_exception -labsl_status -labsl_cord -labsl_cordz_info -labsl_cord_internal -labsl_cordz_functions -labsl_exponential_biased -labsl_cordz_handle -labsl_crc_cord_state -labsl_crc32c -labsl_crc_internal -labsl_crc_cpu_detect -labsl_bad_optional_access -labsl_strerror -labsl_str_format_internal -labsl_synchronization -labsl_graphcycles_internal -labsl_kernel_timeout_internal -labsl_stacktrace -labsl_symbolize -labsl_debugging_internal -labsl_demangle_internal -labsl_malloc_internal -labsl_time -labsl_civil_time -labsl_strings -labsl_strings_internal -labsl_string_view -labsl_base -lrt -labsl_spinlock_wait -labsl_int128 -labsl_throw_delegate -labsl_time_zone -labsl_bad_variant_access -labsl_raw_logging_internal -labsl_log_severity -L/usr/local/include/grpc++ -lgrpc++

.PHONY: grpc-files
grpc-files:
	protoc counter.proto --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin`

.PHONY: proto-files
proto-files:
	protoc counter.proto --cpp_out=.

server: server.cpp server.h
	g++ -o server server.cpp counter.pb.cc counter.grpc.pb.cc $(CC_FLAGS)

client: client.cpp
	g++ -o client client.cpp counter.pb.cc counter.grpc.pb.cc $(CC_FLAGS)
