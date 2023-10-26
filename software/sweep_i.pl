#!/usr/bin/perl
#
# connect to laser control board
# sweep set current and make a plot file of PMON and IMON vs VSET
#
$| = 1;

my $imin = 0;
my $imax = 600;
my $istep = 5;
my $dev = "/dev/ttyUSB0";

my $port;

open( $port, "+<", $dev) or die "opening $dev: $!";


sub read_port {
    my $str = "";
    while(1) {
	my $ch;
	my $rc = sysread( $port, $ch, 1);
	if( $rc == 1) {
	    last if( $ch eq ">");
	    $str .= $ch;
	}
    }
    # find first control character, take remainder
    my $cc = index( $str, "\n");
    my $rs = substr( $str, $cc);
    $rs =~ s/[^[:print:]]+//g;
    return $rs;
}




for( my $ival = $imin; $ival <= $imax; $ival += $istep) {
    print $port "i 0 $ival\n";
    read_port();
    print $port "g 0\n";
    my $res = read_port();
    print "$ival " . $res . "\n";
}


