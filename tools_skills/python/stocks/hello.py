
def initialize(context):
    g.security = '000001.XSHE'
    set_universe([g.security])

def handle_data(context, data):
    security = g.security
    average_price = data[security].mavg(5)
    current_price = data[security].price
    cash = context.portfolio.cash

    if current_price < 0.95*average_price:
        number_of_shares = int(cash/current_price)
        if number_of_shares > 0:
            order(security, +number_of_shares)
            log.info("Buying %s" % (security))
    elif current_price > 1.05*average_price and context.portfolio.positions[security].amount > 0:
        order_target(security, 0)
        log.info("Selling %s" % (security))
    record(stock_price=data[security].price)



