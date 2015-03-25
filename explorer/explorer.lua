
-- ===== CONFIGURATION =============================================================================

CSV_PATH = "/Users/bill/src/cpp_simulator/data/2014.bin"

CHARTS = [[
    candlestick, M1, 1440 | sma,60:sma,12
]]

-- ===== CODE BEGINS ===============================================================================

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

buy_signals = 0
sell_signals = 0

function buy_signal()
    -- a. average close bid over last 1440 minutes:
    local a = average_close_bid()

    -- b. last minute's close bid
    local b = candlestick_M1[0].close_bid

    -- c. std deviation of close bid over last 1440 minutes
    local c = stddev_of_close_bid()

    -- when (a-b)/c between -4 and -7
    local result = (a - b) / c

    if result >= -7 and result <= -4 then
        buy_signals = buy_signals + 1
    end
end

function sell_signal()
end

function before_run()
end

function after_run()
    print("Buy signals: " .. buy_signals)
    print("Sell signals: " .. sell_signals)
end
