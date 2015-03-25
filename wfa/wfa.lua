
-- ===== CONFIGURATION =============================================================================

CSV_PATH = "/Users/bill/src/cpp_simulator/data/2014.bin"
IN_SAMPLE_TIME     = "3 weeks"
OUT_OF_SAMPLE_TIME = "1 week"
OPTIMIZATIONS = 3
STEPS = 3
INITIAL_DEPOSIT = 100.0
MINIMUM_OPTIMIZATION_SCORE = -100.0
MINIMUM_EXECUTION_SCORE = -100.0
MINIMUM_OVERALL_SCORE = 115.0
POST_RUN_SCRIPT = ""
DRAWDOWN_LIMIT = 15.0

CHARTS = [[
    candlestick, M1, 1440 | sma,60:sma,12
]]

VARIABLES = [[
    bool , some_bool
    float, some_float, 1.2, 2.3
    int  , some_int  , 1  , 3
]]

-- ===== CODE BEGINS ==============================================================================

function average_close_bid()
    local total = 0.0
    local count = 0

    for k,v in pairs(candlestick_M1) do
        count = count + 1
        total = total + v.close_bid
    end

    return total / count
end

function stddev_of_close_bid()
    local m
    local vm
    local sum = 0
    local count = 0
    local result

    m = average_close_bid()

    for k,v in pairs(candlestick_M1) do
      vm = v.close_bid - m
      sum = sum + (vm * vm)
      count = count + 1
    end

    result = math.sqrt(sum / (count-1))

    return result
end

function on_tick()
    if has_open_trades then
        if (0 == math.random(0, 7500)) then
            return close_trade()
        else
            return noop()
        end
    end

    -- a. average close bid over last 1440 minutes:
    local a = average_close_bid()

    -- b. last minute's close bid
    local b = candlestick_M1[0].close_bid

    -- c. std deviation of close bid over last 1440 minutes
    local c = stddev_of_close_bid()

    -- when (a-b)/c between -4 and -7
    local result = (a - b) / c

    if result >= -7 and result <= -4 then
        print("Result: " .. result .. " ... going long")
        return buy()
    end

    return noop()
end
