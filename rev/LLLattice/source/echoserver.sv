module echoserver (
	input logic clk,
	input logic n_rst,
	input logic rx,
	output logic tx
);
	logic rx_dv, rx_dv_ff, next_rx_dv_ff, tx_dv;
	logic[7:0] rx_byte, rx_byte_ff, next_rx_byte_ff, tx_byte;

	uart_rx RX (
		.i_Clock(clk),
		.i_Rx_Serial(rx),
		.o_Rx_DV(rx_dv),
		.o_Rx_Byte(rx_byte)
	);

  logic tx_active, tx_serial, tx_done;

	uart_tx TX (
		.i_Clock(clk),
		.i_Tx_DV(tx_dv),
		.i_Tx_Byte(tx_byte),
		.o_Tx_Active(tx_active),
		.o_Tx_Serial(tx),
		.o_Tx_Done(tx_done)
	);

	logic[5:0] state, next_state;
	logic win;

	logic[5:0] transmit_state, next_transmit_state;
	logic ignore, next_ignore;

	assign win = state == 6'd20;

	always_comb begin
		next_rx_dv_ff = rx_dv_ff;
		next_rx_byte_ff = rx_byte_ff;
		next_transmit_state = transmit_state;
		next_state = state;
		tx_byte = rx_byte_ff;
		tx_dv = rx_dv_ff;
		next_ignore = 1'b0;

		if (rx_dv) begin
			case (state)
				6'd0: if (rx_byte == 8'h76) next_state = state + 1; else next_state = 0;
				6'd1: if (rx_byte == 8'h33) next_state = state + 1; else next_state = 0;
				6'd2: if (rx_byte == 8'h72) next_state = state + 1; else next_state = 0;
				6'd3: if (rx_byte == 8'h69) next_state = state + 1; else next_state = 0;
				6'd4: if (rx_byte == 8'h6c) next_state = state + 1; else next_state = 0;
				6'd5: if (rx_byte == 8'h30) next_state = state + 1; else next_state = 0;
				6'd6: if (rx_byte == 8'h67) next_state = state + 1; else next_state = 0;
				6'd7: if (rx_byte == 8'h5f) next_state = state + 1; else next_state = 0;
				6'd8: if (rx_byte == 8'h31) next_state = state + 1; else next_state = 0;
				6'd9: if (rx_byte == 8'h73) next_state = state + 1; else next_state = 0;
				6'd10: if (rx_byte == 8'h5f) next_state = state + 1; else next_state = 0;
				6'd11: if (rx_byte == 8'h70) next_state = state + 1; else next_state = 0;
				6'd12: if (rx_byte == 8'h61) next_state = state + 1; else next_state = 0;
				6'd13: if (rx_byte == 8'h69) next_state = state + 1; else next_state = 0;
				6'd14: if (rx_byte == 8'h6e) next_state = state + 1; else next_state = 0;
				6'd15: if (rx_byte == 8'h5f) next_state = state + 1; else next_state = 0;
				6'd16: if (rx_byte == 8'h70) next_state = state + 1; else next_state = 0;
				6'd17: if (rx_byte == 8'h65) next_state = state + 1; else next_state = 0;
				6'd18: if (rx_byte == 8'h6b) next_state = state + 1; else next_state = 0;
				6'd19: if (rx_byte == 8'h6f) next_state = state + 1; else next_state = 0;
			endcase
		end

		if (!win) begin
			if (rx_dv) begin
				next_rx_dv_ff = 1'b1;
				next_rx_byte_ff = rx_byte;
			end else if (tx_done) begin
				next_rx_dv_ff = 1'b0;
			end
		end else begin
			if (tx_done && !ignore) begin
				if (transmit_state == 40) begin
					next_state = '0;
					next_transmit_state = '0;
					next_rx_dv_ff = 1'b0;
				end else begin
					next_transmit_state = transmit_state + 1;
					next_ignore = 1'b1;
				end
			end

			tx_dv = 1'b1;
			case (transmit_state)
				6'd1: tx_byte = 8'h00;
				6'd2: tx_byte = 8'h70;
				6'd3: tx_byte = 8'h62;
				6'd4: tx_byte = 8'h63;
				6'd5: tx_byte = 8'h74;
				6'd6: tx_byte = 8'h66;
				6'd7: tx_byte = 8'h7b;
				6'd8: tx_byte = 8'h68;
				6'd9: tx_byte = 8'h34;
				6'd10: tx_byte = 8'h72;
				6'd11: tx_byte = 8'h64;
				6'd12: tx_byte = 8'h77;
				6'd13: tx_byte = 8'h61;
				6'd14: tx_byte = 8'h72;
				6'd15: tx_byte = 8'h65;
				6'd16: tx_byte = 8'h5f;
				6'd17: tx_byte = 8'h62;
				6'd18: tx_byte = 8'h61;
				6'd19: tx_byte = 8'h63;
				6'd20: tx_byte = 8'h6b;
				6'd21: tx_byte = 8'h64;
				6'd22: tx_byte = 8'h30;
				6'd23: tx_byte = 8'h30;
				6'd24: tx_byte = 8'h72;
				6'd25: tx_byte = 8'h73;
				6'd26: tx_byte = 8'h5f;
				6'd27: tx_byte = 8'h34;
				6'd28: tx_byte = 8'h72;
				6'd29: tx_byte = 8'h33;
				6'd30: tx_byte = 8'h5f;
				6'd31: tx_byte = 8'h76;
				6'd32: tx_byte = 8'h65;
				6'd33: tx_byte = 8'h72;
				6'd34: tx_byte = 8'h79;
				6'd35: tx_byte = 8'h5f;
				6'd36: tx_byte = 8'h66;
				6'd37: tx_byte = 8'h75;
				6'd38: tx_byte = 8'h6e;
				6'd39: tx_byte = 8'h21;
				6'd40: tx_byte = 8'h7d;
			endcase
		end
	end

	always_ff @ (posedge clk or negedge n_rst) begin
		if (!n_rst) begin
			state <= '0;
			rx_dv_ff <= '0;
			rx_byte_ff <= '0;
			transmit_state <= '0;
			ignore <= '0;
		end else begin
			state <= next_state;
			rx_dv_ff <= next_rx_dv_ff;
			rx_byte_ff <= next_rx_byte_ff;
			transmit_state <= next_transmit_state;
			ignore <= next_ignore;
		end
	end

endmodule
