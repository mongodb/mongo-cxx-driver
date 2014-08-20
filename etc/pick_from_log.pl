#!/usr/bin/perl -w

use strict;

use Term::ANSIColor;
use Term::ReadKey;
use List::Util qw( min max );
use Getopt::Long;

my $cxx;
my $mongo;

GetOptions(
    'cxx=s'   => \$cxx,
    'mongo=s' => \$mongo,
    'help'    => sub { HELP(0); },
) or HELP(1);

$cxx or print "Please enter a path to the mongodb cxx driver repository\n" and HELP(1);
$mongo or print "Please enter a path to the mongodb mongo repository\n" and HELP(1);

my @kept;

my $sha;
my @lines;
my $has_file = 0;
my @decisions;

open FILE, "cd $mongo; git log --stat --oneline --reverse @ARGV |" or die "Couldn't execute git log: $!";

my $total_shas = 0;

while (my $line = <FILE>) {
    if ($line =~ /^\s/) {
        if ($line =~ /^ \d+ files? changed/) {
            push @lines, $line;
        } else {
            my ($path) = ($line =~ /^ (\S+)\s*\|/);
            if (! $path) {
                die $line;
            }

            if (-f "$cxx/$path") {
                $has_file = 1;
                push @lines, color('bold green') . $line . color('reset');
            } else {
                push @lines, color('bold red') . $line . color('reset');
            }
        }
    } else {
        if ($has_file) {
            $total_shas++;
            $has_file = 0;

            push @decisions, [$sha, [@lines]];
        }

        @lines = ();
        ($sha) = ($line =~ /^(\S+)/);

        push @lines, color('bold yellow') . $line . color('reset');
    }
}

close FILE;

if ($has_file) {
    $total_shas++;
    $has_file = 0;

    push @decisions, [$sha, [@lines]];
}

@kept = map { "" } @decisions;

ReadMode 4;

my $seen_shas;
for (my $i = 0; $i < scalar(@decisions); $i++) {
    $seen_shas = $i + 1;

    eval {
        if (! decision($i, @{$decisions[$i]})) {
            $i = max(-1, $i - 2);
        }
    };
    if ($@) {
        ReadMode 0;
        die $@;
    }
}

ReadMode 0;

print join('', map { "$_\n" } @kept);

exit 0;

sub debug {
    warn @_;
    sleep 1;
}

sub decision {
    my ($index, $sha, $lines) = @_;

    my $offset = 0;

    my $y = $kept[$index] ? color('bold green') . 'Y' . color('reset') : 'y';
    my $n = $kept[$index] ? 'n' : color('bold red') . 'N' . color('reset');

    my $options = <<DOC ;
($y) - yes
($n) - no
(s) - show
(b) - back
(q) - quit

DOC

    display($lines, \$offset, $options);

    while (my $key = ReadKey(0)) {
        my %table = (
            'y' => sub {
                $kept[$index] = $sha;
                return 1;
            },
            'n' => sub {
                $kept[$index] = "";
                return 1;
            },
            'b' => sub {
                return 2;
            },
            's' => sub {
                system("cd $mongo; git show $sha");
                return 0;
            },
            'q' => sub {
                ReadMode 0;
                exit 0;
            },
            chr(10) => sub {
                return 1;
            },
            chr(32) => sub {
                return 1;
            },
            chr(27) => sub {
                my $key = ord(ReadKey(0));
                if ($key == 91) {
                    $key = ord(ReadKey(0));

                    if ($key == 65) {
                        $offset--;
                    } elsif ($key == 66) {
                        $offset++;
                    } elsif ($key == 49) {
                        $key = ord(ReadKey(0));

                        if ($key == 126) {
                            $offset = 0;
                        } else {
                            debug "pressed 27.91.49." . $key . "\n";
                        }
                    } elsif ($key == 52) {
                        $key = ord(ReadKey(0));

                        if ($key == 126) {
                            $offset = scalar(@$lines);
                        } else {
                            debug "pressed 27.91.52." . $key . "\n";
                        }
                    } elsif ($key == 53) {
                        $key = ord(ReadKey(0));

                        if ($key == 126) {
                            $offset = max(0, $offset - 10);
                        } else {
                            debug "pressed 27.91.53." . $key . "\n";
                        }
                    } elsif ($key == 54) {
                        $key = ord(ReadKey(0));

                        if ($key == 126) {
                            $offset = min(scalar(@$lines), $offset + 10);
                        } else {
                            debug "pressed 27.91.54." . $key . "\n";
                        }
                    } else {
                        debug "pressed 27.91." . $key . "\n";
                    }
                } else {
                    debug "pressed 27." . $key . "\n";
                }
                return 0;
            },
        );

        if ($table{$key}) {
            my $x = $table{$key}->();

            if ($x == 1) {
                last;
            } elsif ($x == 2) {
                return 0;
            }
        } else {
            debug "pressed " . ord($key) . "\n";
        }

        display($lines, \$offset, $options);
    }

    return 1;
}

sub display {
    my ($lines, $offset, $options) = @_;

    print `clear`;

    my ($width, $height) = GetTerminalSize();
    my $window = $height - (scalar(split /\n/, $options) + 10);

    $$offset = max($$offset, 0);
    $$offset = min($$offset, max(scalar(@$lines) - $window - 1, 0));

    my $min = $$offset;
    my $max = min($window + $$offset, scalar(@$lines)-1);

    printf("%d% (%d/%d)\n", (100 * $seen_shas / $total_shas), $seen_shas, $total_shas);

    print map {
        if (length($_) <= $width) {
            $_;
        } else {
            substr($_, 0, $width - 3) . "...\n";
        }
    } @$lines[$min..$max];

    print "\n", $options;

}

sub HELP {
    my ($exit) = @_;

    my $usage = <<USAGE ;
$0 - [OPTIONS] GIT_DIFF

GIT_DIFF

    A valid git diff range.  I.e. tag1..tag2

OPTIONS

    --cxx=repo   Path to a mongo cxx repo with legacy checked out
    --mongo=repo Path to the main mongo repo which we will generate a git log
                 from

    --help       This help prompt

USAGE

    if ($exit) {
        warn $usage;
    } else {
        print $usage;
    }

    exit $exit;
}
