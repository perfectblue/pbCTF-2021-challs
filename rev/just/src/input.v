
module test(x, out);
    input [63:0] x;
    output [63:0] out;

    reg [63:0] tmp;
    initial begin
        tmp = x;
        tmp = tmp ^ 64'h9445827458d1e38f;
        tmp = tmp + 64'hf053cfc591ae5a85;
        tmp = tmp * 64'hf61475c34efa5845;
        tmp = {tmp, tmp} >> 46;
        tmp = tmp ^ 64'h2fc55b45c61ebe31;
        tmp = tmp + 64'h4e317ba1cc4318f6;
        tmp = tmp * 64'h248ce601f038a07;
        tmp = {tmp, tmp} >> 15;
        tmp = tmp ^ 64'h31a9687a17608c12;
        tmp = tmp + 64'hed0c25e75b89aced;
        tmp = tmp * 64'h975bf348d59b263b;
        tmp = {tmp, tmp} >> 41;
        tmp = tmp ^ 64'hc6ef667a38bdce09;
        tmp = tmp + 64'h71122c73403c6eb9;
        tmp = tmp * 64'hcccb0c9343f99e2b;
        tmp = {tmp, tmp} >> 29;
        tmp = tmp ^ 64'hb6d5b0156ab6173c;
        tmp = tmp + 64'hc1dc47474a382c47;
        tmp = tmp * 64'h684a6a759995e6bd;
        tmp = {tmp, tmp} >> 4;
        tmp = tmp ^ 64'hf7717a58c7bb2d9e;
        tmp = tmp + 64'hfc8209b7ca52282a;
        tmp = tmp * 64'h2613486a2bb075b9;
        tmp = {tmp, tmp} >> 44;
        tmp = tmp ^ 64'hf665597571bc48d;
        tmp = tmp + 64'h85334c2b17e4655a;
        tmp = tmp * 64'h91e7d7118a7c3b1f;
        tmp = {tmp, tmp} >> 1;
        tmp = tmp ^ 64'h473f56b61d111668;
        tmp = tmp + 64'h60fd0697f9c18b48;
        tmp = tmp * 64'he741548ac2acdc99;
        tmp = {tmp, tmp} >> 44;
    end
    assign out = tmp;
endmodule
