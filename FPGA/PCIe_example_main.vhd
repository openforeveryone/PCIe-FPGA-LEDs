library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

entity PCIE_Example_Main  is
	port (
				--inputs
				CLOCK_100_p : in std_logic;
				dips : in unsigned(3 downto 0);
				buttons : in unsigned(2 downto 0);
				rx_serial_0_p : in std_logic;
				PCIE_PERSTn : in std_logic;
				--outputs
				leds : out std_logic_vector(3 downto 0);
				tx_serial_0_p : out std_logic
			);
end entity PCIE_Example_Main;

architecture PCIE_Example_arch of PCIE_Example_Main is

component pcie_ed is
	port (
		board_pins_L0_led                : out std_logic;                                        --                board_pins.L0_led
		board_pins_alive_led             : out std_logic;                                        --                          .alive_led
		board_pins_comp_led              : out std_logic;                                        --                          .comp_led
		board_pins_free_clk              : in  std_logic                     := '0';             --                          .free_clk
		board_pins_gen2_led              : out std_logic;                                        --                          .gen2_led
		board_pins_gen3_led              : out std_logic;                                        --                          .gen3_led
		board_pins_lane_active_led       : out std_logic_vector(3 downto 0);                     --                          .lane_active_led
		board_pins_req_compliance_pb     : in  std_logic                     := '0';             --                          .req_compliance_pb
		board_pins_set_compliance_mode   : in  std_logic                     := '0';             --                          .set_compliance_mode
		refclk_clk                       : in  std_logic                     := '0';             --                    refclk.clk
		pcie_rstn_npor                   : in  std_logic                     := '0';             --                 pcie_rstn.npor
		pcie_rstn_pin_perst              : in  std_logic                     := '0';             --                          .pin_perst
		hip_ctrl_test_in                 : in  std_logic_vector(31 downto 0) := (others => '0'); --                  hip_ctrl.test_in
		hip_ctrl_simu_mode_pipe          : in  std_logic                     := '0';             --                          .simu_mode_pipe
		pipe_sim_only_sim_pipe_pclk_in   : in  std_logic                     := '0';             --             pipe_sim_only.sim_pipe_pclk_in
		pipe_sim_only_sim_pipe_rate      : out std_logic_vector(1 downto 0);                     --                          .sim_pipe_rate
		pipe_sim_only_sim_ltssmstate     : out std_logic_vector(4 downto 0);                     --                          .sim_ltssmstate
		pipe_sim_only_eidleinfersel0     : out std_logic_vector(2 downto 0);                     --                          .eidleinfersel0
		pipe_sim_only_powerdown0         : out std_logic_vector(1 downto 0);                     --                          .powerdown0
		pipe_sim_only_rxpolarity0        : out std_logic;                                        --                          .rxpolarity0
		pipe_sim_only_txcompl0           : out std_logic;                                        --                          .txcompl0
		pipe_sim_only_txdata0            : out std_logic_vector(31 downto 0);                    --                          .txdata0
		pipe_sim_only_txdatak0           : out std_logic_vector(3 downto 0);                     --                          .txdatak0
		pipe_sim_only_txdetectrx0        : out std_logic;                                        --                          .txdetectrx0
		pipe_sim_only_txelecidle0        : out std_logic;                                        --                          .txelecidle0
		pipe_sim_only_txdeemph0          : out std_logic;                                        --                          .txdeemph0
		pipe_sim_only_txmargin0          : out std_logic_vector(2 downto 0);                     --                          .txmargin0
		pipe_sim_only_txswing0           : out std_logic;                                        --                          .txswing0
		pipe_sim_only_phystatus0         : in  std_logic                     := '0';             --                          .phystatus0
		pipe_sim_only_rxdata0            : in  std_logic_vector(31 downto 0) := (others => '0'); --                          .rxdata0
		pipe_sim_only_rxdatak0           : in  std_logic_vector(3 downto 0)  := (others => '0'); --                          .rxdatak0
		pipe_sim_only_rxelecidle0        : in  std_logic                     := '0';             --                          .rxelecidle0
		pipe_sim_only_rxstatus0          : in  std_logic_vector(2 downto 0)  := (others => '0'); --                          .rxstatus0
		pipe_sim_only_rxvalid0           : in  std_logic                     := '0';             --                          .rxvalid0
		pipe_sim_only_rxdataskip0        : in  std_logic                     := '0';             --                          .rxdataskip0
		pipe_sim_only_rxblkst0           : in  std_logic                     := '0';             --                          .rxblkst0
		pipe_sim_only_rxsynchd0          : in  std_logic_vector(1 downto 0)  := (others => '0'); --                          .rxsynchd0
		pipe_sim_only_currentcoeff0      : out std_logic_vector(17 downto 0);                    --                          .currentcoeff0
		pipe_sim_only_currentrxpreset0   : out std_logic_vector(2 downto 0);                     --                          .currentrxpreset0
		pipe_sim_only_txsynchd0          : out std_logic_vector(1 downto 0);                     --                          .txsynchd0
		pipe_sim_only_txblkst0           : out std_logic;                                        --                          .txblkst0
		pipe_sim_only_txdataskip0        : out std_logic;                                        --                          .txdataskip0
		pipe_sim_only_rate0              : out std_logic_vector(1 downto 0);                     --                          .rate0
		xcvr_rx_in0                      : in  std_logic                     := '0';             --                      xcvr.rx_in0
		xcvr_tx_out0                     : out std_logic;                                        --                          .tx_out0
		pio_0_external_connection_export : out std_logic_vector(3 downto 0)                      -- pio_0_external_connection.export
	);
end component pcie_ed;
	
component SFL is
	port (
		noe_in : in std_logic := '0'  -- noe_in.noe
	);
end component SFL;
	
begin	
	Mypcie_ed : pcie_ed port map (	
		refclk_clk => CLOCK_100_p,
		pcie_rstn_npor => PCIE_PERSTn,
		pcie_rstn_pin_perst => PCIE_PERSTn,
		xcvr_rx_in0 => rx_serial_0_p,
		xcvr_tx_out0 => tx_serial_0_p,
		pio_0_external_connection_export => leds
	);
end architecture PCIE_Example_arch;