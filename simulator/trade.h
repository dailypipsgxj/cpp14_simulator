#ifndef TRADE_H
#define TRADE_H

#include <vector>

#include "tick.h"

class Trade {
protected:
    unsigned int m_id; // TODO: so we can close trades by ID
    bool m_open; // so we can properly calculate profit()

    time_t m_opened_at;
    time_t m_closed_at;

    float m_filled_open_price;
    float m_filled_close_price;
    // float m_desired_open_price;
    // float m_desired_close_price;

    // float m_allowed_slippage;

    // std::vector<std::shared_ptr<StopLoss>> m_stop_losses;
    // std::vector<std::shared_ptr<TakeProfit>> m_take_profits;

    // bool m_is_stop_loss_hit;
    // bool m_is_take_profit_hit;

    // for calculating spreads and actual slippage
    float m_open_bid;
    float m_open_ask;
    float m_close_bid;
    float m_close_ask;

    // extra fields for debugging / analytics
    // float m_equity_at_open;
    // float m_equity_at_close;
    // float m_balance_at_open;
    // float m_balance_at_close;

    // float lowest_bid;
    // float lowest_ask;
    // float highest_bid;
    // float highest_ask;

    // OrdersOpenAtOpen  int64
    // OrdersOpenAtClose int64

    // float drawdown_at_open;
    // float drawdown_at_close;

    // float closest_percentage_to_take_profit;
    // float closest_percentage_to_stop_loss;

private:
    std::vector<std::shared_ptr<Tick>> m_ticks;

public:
    Trade(unsigned int, std::shared_ptr<Tick>);
    virtual ~Trade();
    virtual void close(std::shared_ptr<Tick>) = 0;
    void record_tick(std::shared_ptr<Tick>);
    virtual float profit() = 0;
    std::shared_ptr<Tick> get_last_tick();
    unsigned long time_open();

    // TODO: CSV functions
};

#endif
