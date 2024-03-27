module converter (
    input wire f0,
    input wire c4,
    input wire select,
    output reg clk_en_tx,
    output reg clk_en_rx,
    output reg clk_tx,
    output reg clk_rx
);
integer i,j,k,l;

always @ (negedge f0)
   begin
      clk_en_tx <= 1'b1;
      @ (negedge c4)
      begin
         #0 clk_tx <= 1'b1;
         for (i=0; i<30; i=i+1) 
            begin
               #244 clk_tx <= 1'b0;
               #244 clk_tx <= 1'b1;
            end
         #244 clk_tx <= 1'b0;   
         #122 clk_en_tx <= 1'b0; 
         #122 clk_tx <= 1'b1; 
         #244 clk_tx <= 1'b0;  
      end
   end

always @ (negedge f0)
   begin
      clk_en_rx <= 1'b1;
      @ (negedge c4)
      begin
         #366 clk_rx <= 1'b1;
         for (j=0; j<31; j=j+1) 
            begin
               #244 clk_rx <= 1'b0;
               #244 clk_rx <= 1'b1;
            end
         #244 clk_rx <= 1'b0;   
         #122 clk_en_rx <= 1'b0; 
      end
   end

// always @ (negedge f0)
//    begin
//       clk_en_rx <= 1'b1;
//       @ (negedge c4)
//       begin
//          #244 clk_rx <= 1'b1;
//          for (k=0; k<31; k=k+1) 
//             begin
//                #244 clk_rx <= 1'b0;
//                #244 clk_rx <= 1'b1;
//             end
//          #244 clk_rx <= 1'b0;   
//          #122 clk_en_rx <= 1'b0; 
//       end
//    end

// always @ (negedge f0)
//    begin
//       clk_en_tx <= 1'b1;
//       @ (negedge c4)
//       begin
//          #244 clk_tx <= 1'b1;
//          for (l=0; l<30; l=l+1) 
//             begin
//                #244 clk_tx <= 1'b0;
//                #244 clk_tx <= 1'b1;
//             end
//          #244 clk_tx <= 1'b0;   
//          #122 clk_en_tx <= 1'b0; 
//          #122 clk_tx <= 1'b1; 
//          #244 clk_tx <= 1'b0;  
//       end
//    end 
endmodule
