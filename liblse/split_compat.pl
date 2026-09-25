#!/usr/bin/env perl
use strict;
use warnings;
use File::Basename;

my $input_file = shift @ARGV or die "Usage: $0 <input_compat.h>\n";
open(my $fh, '<', $input_file) or die "Cannot open $input_file: $!\n";

my $base_name = fileparse($input_file, qr/\.[^.]*/);
my $header_file = "${base_name}.h";
my $c_file      = "${base_name}.c";

open(my $h_out, '>', $header_file) or die "Cannot write $header_file: $!\n";
open(my $c_out, '>', $c_file)      or die "Cannot write $c_file: $!\n";

my $guard = uc("${base_name}_H");
$guard =~ s/[^A-Z0-9_]/_/g;

print $h_out "#ifndef $guard\n#define $guard\n\n";
print $h_out "#include <sys/types.h>\n";

# inject networking headers ONLY for liblsenet components
#if ($base_name eq 'socket_compat' || $base_name eq 'dns_rfc2553_compat') {
#    print $h_out "#include <sys/socket.h>\n";
#    print $h_out "#include <netdb.h>\n";
#}
print $h_out "\n";

print $c_out "#include \"$header_file\"\n";
print $c_out "#include <stdio.h>\n";
print $c_out "#include <stdlib.h>\n";
print $c_out "#include <string.h>\n\n";

my $in_impl_block = 0;
my $found_impl    = 0;

while (my $line = <$fh>) {
    if ($line =~ /\/\*\s*BEGIN_IMPL\s*\*\// || $line =~ /^static\s+inline/ || $line =~ /^#ifdef\s+COMPAT_IMPLEMENTATION/) {
        $in_impl_block = 1;
        $found_impl    = 1;
        next;
    }
    if ($line =~ /\/\*\s*END_IMPL\s*\*\// || $line =~ /^#endif\s*\/\*\s*COMPAT_IMPLEMENTATION\s*\*\//) {
        $in_impl_block = 0;
        next;
    }

    if ($in_impl_block) {
        $line =~ s/^static\s+inline\s+//;
        print $c_out $line;
    } else {
        # Escape hatch to force a header into the .h file
        if ($line =~ /\/\*\s*KEEP\s*\*\//) {
            print $h_out $line;
        }
        # Intercept system includes in the header section to prevent header pollution during -include
        if ($line =~ /^\s*#\s*include\s+<([^>]+)>/) {
            my $header_name = $1;
            # Allow safe type headers into the exported .h
            if ($header_name =~ /^(stddef\.h|sys\/types\.h|stdarg\.h|stdint\.h|inttypes\.h)$/) {
                print $h_out $line;
            } else {
                # Push standard library headers (stdio.h, stdlib.h, string.h, etc.) to the .c implementation
                print $c_out $line;
            }
        } else {
            # Print normal typedefs, macros, and function prototypes to the .h header
            print $h_out $line;
        }
    }
}

print $h_out "\n#endif /* $guard */\n";

close($fh);
close($h_out);
close($c_out);

if ($found_impl) {
    print "Split $input_file into $header_file and $c_file\n";
} else {
    unlink($c_file); # Delete empty C file
    print "Pure header detected ($input_file). Generated $header_file (deleted empty $c_file).\n";
}
