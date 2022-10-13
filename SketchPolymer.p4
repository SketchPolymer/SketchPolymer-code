// For simplicity, we use 1 hash function for each stage and choose a = 2


#include <core.p4>
#include <tna.p4>


typedef bit<48> mac_addr_t;

const bit<32> STAGE1_SIZE = 1 << 15;
const bit<32> STAGE2_SIZE = 1 << 15;
const bit<32> STAGE3_SIZE = 1 << 18;
const bit<32> STAGE4_SIZE = 1 << 19;

typedef bit<15> stage1_length_t;
typedef bit<15> stage2_length_t;
typedef bit<18> stage3_length_t;
typedef bit<19> stage4_length_t;

enum bit<16> ether_type_t {
    TPID       = 0x8100,
    IPV4       = 0x0800
}

/*************************************************************************
 ***********************  H E A D E R S  *********************************
 *************************************************************************/
/*  Define all the headers the program will recognize             */
/*  The actual sets of headers processed by each gress can differ */

/* Standard ethernet header */
header ethernet_h {
    mac_addr_t    dst_addr;
    mac_addr_t    src_addr;
    ether_type_t  ether_type;
}


header ipv4_h {
    bit<4>   version;
    bit<4>   ihl;
    bit<8>   diffserv;
    bit<16>  total_len;
    bit<16>  identification;
    bit<3>   flags;
    bit<13>  frag_offset;
    bit<8>   ttl;
    bit<8>   protocol;
    bit<16>  hdr_checksum;
    bit<32>  src_addr;
    bit<32>  dst_addr;
}

header udp_h {
    bit<16>  src_port;
    bit<16>  dst_port;
    bit<16>  len;
    bit<16>  checksum;
}

header my_flow_h {
	bit<64> id;
	bit<64> latency;
}


/*************************************************************************
 **************  I N G R E S S   P R O C E S S I N G   *******************
 *************************************************************************/
 
    /***********************  H E A D E R S  ************************/

struct my_ingress_headers_t {
    ethernet_h         ethernet;

    ipv4_h             ipv4;
    udp_h              udp;
	my_flow_h          myflow;
}

    /******  G L O B A L   I N G R E S S   M E T A D A T A  *********/

struct flow_id_t {
	bit<64> id;
	bit<64> latency;
}

struct my_ingress_metadata_t {
    flow_id_t        flow_id;
    stage1_length_t  stage1_index;
	bit<32>          log_latency;
	stage2_length_t  stage2_index;
	stage3_length_t  stage3_index;
	stage4_length_t  stage4_index;
}


    /***********************  P A R S E R  **************************/

parser IngressParser(packet_in        pkt,
    /* User */
    out my_ingress_headers_t          hdr,
    out my_ingress_metadata_t         meta,
    /* Intrinsic */
    out ingress_intrinsic_metadata_t  ig_intr_md)
{
    /* This is a mandatory state, required by Tofino Architecture */
    state start {
        pkt.extract(ig_intr_md);
        pkt.advance(PORT_METADATA_SIZE);
        transition parse_ethernet;
    }

    state parse_ethernet {
        pkt.extract(hdr.ethernet);
        /* 
         * The explicit cast allows us to use ternary matching on
         * serializable enum
         */        
        transition select((bit<16>)hdr.ethernet.ether_type) {
            (bit<16>)ether_type_t.IPV4            :  parse_ipv4;
            default :  accept;
        }
    }

    state parse_ipv4 {
        pkt.extract(hdr.ipv4);
        transition select(hdr.ipv4.protocol) {
            17 : parse_udp;
            default : accept;
	    }
    }

    state parse_udp {
        pkt.extract(hdr.udp);
        transition parse_myflow;
	}
	
	state parse_myflow{
		pkt.extract(hdr.myflow);
		meta.flow_id.id = hdr.myflow.id;
		
		// meta.flow_id.latency = hdr.myflow.latency;
		transition accept;
	}
}


control Ingress(/* User */
    inout my_ingress_headers_t                       hdr,
    inout my_ingress_metadata_t                      meta,
    /* Intrinsic */
    in    ingress_intrinsic_metadata_t               ig_intr_md,
    in    ingress_intrinsic_metadata_from_parser_t   ig_prsr_md,
    inout ingress_intrinsic_metadata_for_deparser_t  ig_dprsr_md,
    inout ingress_intrinsic_metadata_for_tm_t        ig_tm_md) 
{

	CRCPolynomial<bit<32>>(0x11111111,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32_stage1;
    CRCPolynomial<bit<32>>(0x12222222,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32_stage2;
    CRCPolynomial<bit<32>>(0x13333333,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32_stage3;
	CRCPolynomial<bit<32>>(0x13333333,true,false,false,32w0xFFFFFFFF,32w0xFFFFFFFF) crc32_stage4;


	Hash<stage1_length_t>(HashAlgorithm_t.CRC32,crc32_stage1) hash_stage1;
    Hash<stage2_length_t>(HashAlgorithm_t.CRC32,crc32_stage2) hash_stage2;
    Hash<stage3_length_t>(HashAlgorithm_t.CRC32,crc32_stage3) hash_stage3;
    Hash<stage4_length_t>(HashAlgorithm_t.CRC32,crc32_stage4) hash_stage4;

	bit<32> result_stage1;
	// bit<32> result_stage2;
	// bit<32> result_stage3;
	// bit<32> result_stage4;
    // bit<64> stage2_frequency;
    // bit<64> stage2_latency;

    Register<bit<8>, stage1_length_t>(0x4000) stage1_register;
	RegisterAction<bit<8>, stage1_length_t, bit<32>>(stage1_register) stage1_insert=
    {
        void apply(inout bit<8> register_data, out bit<32> result) 
        {
        	if (register_data<50)
        	{
            	register_data=register_data+1;
        	}
			result = (bit<32>) register_data;
        }
    };

    Register<bit<32>, stage2_length_t>(0x4000) stage2_frequency_register;
	RegisterAction<bit<32>, stage2_length_t, bit<32>>(stage2_frequency_register) stage2_frequency_insert=
    {
        void apply(inout bit<32> register_data, out bit<32> result) 
        {
            register_data = register_data + 1;
        }
    };

    Register<bit<32>, stage2_length_t>(0x4000) stage2_latency_register;
	RegisterAction<bit<32>, stage2_length_t, bit<32>>(stage2_latency_register) stage2_latency_insert=
    {
        void apply(inout bit<32> register_data, out bit<32> result) 
        {
            if (register_data < meta.log_latency) {
                register_data = meta.log_latency;
            }
        }
    };
	
	Register<bit<8>, stage3_length_t>(0x8000) stage3_register;
	RegisterAction<bit<8>, stage3_length_t, bit<32>>(stage3_register) stage3_insert=
	{
		void apply(inout bit<8> register_data, out bit<32> result) 
        {
        	if (register_data<255)
        	{
            	register_data=register_data + 1;
        	}
			// result = register_data;
        }
	};

	Register<bit<1>, stage4_length_t>(0x10000) stage4_register;
	RegisterAction<bit<1>, stage4_length_t, bit<32>>(stage4_register) stage4_insert=
	{
		void apply(inout bit<1> register_data, out bit<32> result) 
        {
        	register_data = 1;
			// result = register_data;
        }
	};

	action cal_index_stage1()
    {
        meta.stage1_index = hash_stage1.get({meta.flow_id.id});
    }
	table cal_index_stage1_t
    {
        actions={cal_index_stage1;}
        default_action=cal_index_stage1;
    }

	action cal_index_stage2()
    {
        meta.stage2_index = hash_stage2.get({meta.flow_id.id, meta.log_latency});
    }
	table cal_index_stage2_t
    {
        actions={cal_index_stage2;}
        default_action=cal_index_stage2;
    }

	action cal_index_stage3()
    {
        meta.stage3_index = hash_stage3.get({meta.flow_id.id, meta.log_latency});
    }
	table cal_index_stage3_t
    {
        actions={cal_index_stage3;}
        default_action=cal_index_stage3;
    }

	action cal_index_stage4()
    {
        meta.stage4_index = hash_stage4.get({meta.flow_id.id, meta.log_latency});
    }
	table cal_index_stage4_t
	{
        actions={cal_index_stage4;}
        default_action=cal_index_stage4;
    }

	action cal_log_latency(bit<32> log_latency) 
	{
		// calculate log of latency here
        meta.log_latency = log_latency;
	}
	table cal_log_latency_t 
	{
        key = {
            meta.flow_id.latency : ternary;
        }
		actions={
            cal_log_latency;
        }
		default_action=cal_log_latency(10);

        // const entries = {
        //     0x8000000000000000 &&& 0x8000000000000000 : apply(63);
        //     0x4000000000000000 &&& 0x4000000000000000 : apply(62);
        //     0x2000000000000000 &&& 0x2000000000000000 : apply(61);
        //     0x1000000000000000 &&& 0x1000000000000000 : apply(60);
        //     0x0800000000000000 &&& 0x0800000000000000 : apply(59);
        //     0x0400000000000000 &&& 0x0400000000000000 : apply(58);
        //     0x0200000000000000 &&& 0x0200000000000000 : apply(57);
        //     0x0100000000000000 &&& 0x0100000000000000 : apply(56);
        //     0x0080000000000000 &&& 0x0080000000000000 : apply(55);
        //     0x0040000000000000 &&& 0x0040000000000000 : apply(54);
        //     0x0020000000000000 &&& 0x0020000000000000 : apply(53);
        //     0x0010000000000000 &&& 0x0010000000000000 : apply(52);
        //     0x0008000000000000 &&& 0x0008000000000000 : apply(51);
        //     0x0004000000000000 &&& 0x0004000000000000 : apply(50);
        //     0x0002000000000000 &&& 0x0002000000000000 : apply(49);
        //     0x0001000000000000 &&& 0x0001000000000000 : apply(48);
        //     0x0000800000000000 &&& 0x0000800000000000 : apply(47);
        //     0x0000400000000000 &&& 0x0000400000000000 : apply(46);
        //     0x0000200000000000 &&& 0x0000200000000000 : apply(45);
        //     0x0000100000000000 &&& 0x0000100000000000 : apply(44);
        //     0x0000080000000000 &&& 0x0000080000000000 : apply(43);
        //     0x0000040000000000 &&& 0x0000040000000000 : apply(42);
        //     0x0000020000000000 &&& 0x0000020000000000 : apply(41);
        //     0x0000010000000000 &&& 0x0000010000000000 : apply(40);
        //     0x0000008000000000 &&& 0x0000008000000000 : apply(39);
        //     0x0000004000000000 &&& 0x0000004000000000 : apply(38);
        //     0x0000002000000000 &&& 0x0000002000000000 : apply(37);
        //     0x0000001000000000 &&& 0x0000001000000000 : apply(36);
        //     0x0000000800000000 &&& 0x0000000800000000 : apply(35);
        //     0x0000000400000000 &&& 0x0000000400000000 : apply(34);
        //     0x0000000200000000 &&& 0x0000000200000000 : apply(33);
        //     0x0000000100000000 &&& 0x0000000100000000 : apply(32);
        //     0x0000000080000000 &&& 0x0000000080000000 : apply(31);
        //     0x0000000040000000 &&& 0x0000000040000000 : apply(30);
        //     0x0000000020000000 &&& 0x0000000020000000 : apply(29);
        //     0x0000000010000000 &&& 0x0000000010000000 : apply(28);
        //     0x0000000008000000 &&& 0x0000000008000000 : apply(27);
        //     0x0000000004000000 &&& 0x0000000004000000 : apply(26);
        //     0x0000000002000000 &&& 0x0000000002000000 : apply(25);
        //     0x0000000001000000 &&& 0x0000000001000000 : apply(24);
        //     0x0000000000800000 &&& 0x0000000000800000 : apply(23);
        //     0x0000000000400000 &&& 0x0000000000400000 : apply(22);
        //     0x0000000000200000 &&& 0x0000000000200000 : apply(21);
        //     0x0000000000100000 &&& 0x0000000000100000 : apply(20);
        //     0x0000000000080000 &&& 0x0000000000080000 : apply(19);
        //     0x0000000000040000 &&& 0x0000000000040000 : apply(18);
        //     0x0000000000020000 &&& 0x0000000000020000 : apply(17);
        //     0x0000000000010000 &&& 0x0000000000010000 : apply(16);
        //     0x0000000000008000 &&& 0x0000000000008000 : apply(15);
        //     0x0000000000004000 &&& 0x0000000000004000 : apply(14);
        //     0x0000000000002000 &&& 0x0000000000002000 : apply(13);
        //     0x0000000000001000 &&& 0x0000000000001000 : apply(12);
        //     0x0000000000000800 &&& 0x0000000000000800 : apply(11);
        //     0x0000000000000400 &&& 0x0000000000000400 : apply(10);
        //     0x0000000000000200 &&& 0x0000000000000200 : apply(9);
        //     0x0000000000000100 &&& 0x0000000000000100 : apply(8);
        //     0x0000000000000080 &&& 0x0000000000000080 : apply(7);
        //     0x0000000000000040 &&& 0x0000000000000040 : apply(6);
        //     0x0000000000000020 &&& 0x0000000000000020 : apply(5);
        //     0x0000000000000010 &&& 0x0000000000000010 : apply(4);
        //     0x0000000000000008 &&& 0x0000000000000008 : apply(3);
        //     0x0000000000000004 &&& 0x0000000000000004 : apply(2);
        //     0x0000000000000002 &&& 0x0000000000000002 : apply(1);
        // }
	}

	apply {
		cal_index_stage1_t.apply();
		result_stage1 = stage1_insert.execute(meta.stage1_index);
		if (result_stage1 == 50) { // large flow
			cal_log_latency_t.apply();
			cal_index_stage2_t.apply();
			cal_index_stage3_t.apply();
			cal_index_stage4_t.apply();
            stage2_frequency_insert.execute(meta.stage2_index);
            stage2_latency_insert.execute(meta.stage2_index);
            stage3_insert.execute(meta.stage3_index);
            stage4_insert.execute(meta.stage4_index);
		}
	}
}



control IngressDeparser(
	packet_out pkt,
    /* User */
    inout my_ingress_headers_t                       hdr,
    in    my_ingress_metadata_t                      meta,
    /* Intrinsic */
    in    ingress_intrinsic_metadata_for_deparser_t  ig_dprsr_md)
{
    apply {
        pkt.emit(hdr);
    }
}

/*************************************************************************
 ****************  E G R E S S   P R O C E S S I N G   *******************
 *************************************************************************/

    /***********************  H E A D E R S  ************************/

struct my_egress_headers_t {
}

    /********  G L O B A L   E G R E S S   M E T A D A T A  *********/

struct my_egress_metadata_t {
}

    /***********************  P A R S E R  **************************/

parser EgressParser(
	packet_in        pkt,
    /* User */
    out my_egress_headers_t          hdr,
    out my_egress_metadata_t         meta,
    /* Intrinsic */
    out egress_intrinsic_metadata_t  eg_intr_md)
{
    /* This is a mandatory state, required by Tofino Architecture */
    state start {
        pkt.extract(eg_intr_md);
        transition accept;
    }
}

    /***************** M A T C H - A C T I O N  *********************/

control Egress(
    /* User */
    inout my_egress_headers_t                          hdr,
    inout my_egress_metadata_t                         meta,
    /* Intrinsic */    
    in    egress_intrinsic_metadata_t                  eg_intr_md,
    in    egress_intrinsic_metadata_from_parser_t      eg_prsr_md,
    inout egress_intrinsic_metadata_for_deparser_t     eg_dprsr_md,
    inout egress_intrinsic_metadata_for_output_port_t  eg_oport_md)
{
    apply {
    }
}

    /*********************  D E P A R S E R  ************************/

control EgressDeparser(packet_out pkt,
    /* User */
    inout my_egress_headers_t                       hdr,
    in    my_egress_metadata_t                      meta,
    /* Intrinsic */
    in    egress_intrinsic_metadata_for_deparser_t  eg_dprsr_md)
{
    apply {
        pkt.emit(hdr);
    }
}


/************ F I N A L   P A C K A G E ******************************/
Pipeline(
    IngressParser(),
    Ingress(),
    IngressDeparser(),
    EgressParser(),
    Egress(),
    EgressDeparser()
) pipe;

Switch(pipe) main;
