#include "orderWrapper.h"

/**
 * This file is provided to perform some common functions that are generally useful in algos.
 * User can change code as per their requirement and apply neccessary checks to minimize risk
 * and maximize performance of application.
 * These functions may not fullfill all the requirements. So, user should check the implementation and 
 * documentation before use.
 * 
 * Disclaimer: uTrade will not be responsible for any issue due to this code as implementation is also provided
 *
 */


namespace API2 {
  namespace CUSTOM_COMMON {

    void LegDetail::updateOrderWrapper(SIGNED_LONG qty,SIGNED_LONG price,SIGNED_LONG lastFilledQty)
    {
      _lastQuotedPrice = price;
      _lastQuantity = qty;
      _lastFilledQuantity += lastFilledQty;
    }
    void LegDetail::resetOrderWrapper()
    {
      _lastQuotedPrice = 0;
      _lastQuantity = 0;
      _lastFilledQuantity = 0;
    }

    void LegDetail::reset()
    {
      if(!instrument){
        DEBUG_MESSAGE(_context->reqQryDebugLog(),"Instrument Null for Leg");
        _context->reqTerminateStrategy(true);
      }
      order = _context->createNewOrder(instrument,0,0,_mode,_orderType, _orderValidity, _productType);
      _isReset = true;
    }

    void OrderWrapper::reset()
    {
      for(std::vector<LegDetail>::iterator iter = _orderLegData.begin(); iter!=_orderLegData.end();iter++)
      {
        LegDetail &leg = *iter;
        leg.reset();
      }
      _orderId = getLeg1OrderId();
      _isReset = true;
    }
    bool OrderWrapper::newOrder(
        API2::DATA_TYPES::RiskStatus &risk,
        const API2::DATA_TYPES::PRICE &priceLeg1,
        const API2::DATA_TYPES::QTY &qtyLeg1,
        const API2::DATA_TYPES::PRICE &priceLeg2,
        const API2::DATA_TYPES::QTY &qtyLeg2,
        const API2::DATA_TYPES::PRICE &priceLeg3,
        const API2::DATA_TYPES::QTY &qtyLeg3
        )
    {
      if(!_instrument)
        return false;
      _isReset = false;


      LegDetail *leg1 =0, *leg2=0, *leg3 =0;
      if(_numLegs>=1)
      {
        if(qtyLeg1 == 0)
        {
          risk = API2::CONSTANTS::RSP_RiskStatus_WRONG_QUANTITY;
          return false;
        }
        leg1 = &_orderLegData[0];
        leg1->order->setQuantity(qtyLeg1);
        leg1->order->setPrice(priceLeg1);
      }
      else
      {
        DEBUG_MESSAGE(_context->reqQryDebugLog(), "Order Wrapper Not Set. num Leg <1. Terminating strategy");
        _context->reqAddStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        _context->reqTerminateStrategy();
        return false;
      }
      if(_numLegs>=2 && _orderLegData.size() >=2)
      {
        if(qtyLeg2 == 0)
        {
          risk = API2::CONSTANTS::RSP_RiskStatus_WRONG_QUANTITY;
          DEBUG_MESSAGE(_context->reqQryDebugLog(), "qty leg 2 0");
          return false;
        }

        leg2 = &_orderLegData[1];
        leg2->order->setQuantity(qtyLeg2);
        leg2->order->setPrice(priceLeg2);
      }
      if(_numLegs==3 && _orderLegData.size() ==3)
      {
        if(qtyLeg3 == 0)
        {
          risk = API2::CONSTANTS::RSP_RiskStatus_WRONG_QUANTITY;
          DEBUG_MESSAGE(_context->reqQryDebugLog(), "returning false qty leg3");
          return false;
        }
        leg3 = &_orderLegData[2];
        leg3->order->setQuantity(qtyLeg3);
        leg3->order->setPrice(priceLeg3);
      }

      if(_numLegs ==1)
      {
        if(leg1){
          if(_context->reqNewSingleOrder(risk,leg1->instrument,leg1->order,leg1->orderId,_isSpread,_orderPriority))
          {
            _isPendingNew = true;
            return true;
          }
        }
        else
        {
          DEBUG_MESSAGE(_context->reqQryDebugLog(), "Order Wrapper Not Set. Terminating strategy");
          DEBUG_MESSAGE(_context->reqQryDebugLog(),"Leg 1 null");
          _context->reqAddStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
          _context->reqTerminateStrategy();
          return false;
        }
      }
      else if(_numLegs >= 2)
      {

        std::vector<SGContext::OrderLegData> legData;
        for(std::vector<LegDetail>::iterator iter = _orderLegData.begin(); iter!= _orderLegData.end();iter++)
        {
          LegDetail legD = *iter;
          SGContext::OrderLegData leg(legD.instrument);
          leg.order = legD.order;
          leg.orderId = legD.orderId;
          if(leg.order && leg.orderId)
            legData.push_back(legD);
        }
        if( _context->reqNewMultilegOrder(risk,legData) )
        {
          _isPendingNew = true;
          return true;
        }
      }
      return false;
    }

    bool OrderWrapper::replaceOrder(DATA_TYPES::RiskStatus &risk, const DATA_TYPES::PRICE &price, const DATA_TYPES::QTY &qty)
    {
      if(!_instrument)
        return false;
      if(_orderLegData.size()>1){
        std::cout << "Cannot Modify Multileg Order" << std::endl;
        return false;
      }
      LegDetail &leg1 = _orderLegData[0];

      if(!leg1.orderId){
        DEBUG_MESSAGE(_context->reqQryDebugLog(), "OrderWrapper replace fail OrderId null");
        return false;
      }
      _isReset = false;

      API2::SingleOrder *order = _context->createNewOrder(leg1.instrument,0,0,leg1._mode,
          leg1._orderType, leg1._orderValidity, leg1._productType);
      if(order)
      {
        API2::SingleOrder *origOrder = _context->reqQryOrder(_context->reqQryClientOrderId(leg1.orderId));
        if(origOrder)
        {
          if(price !=0)
            order->setPrice(price);
          else
            order->setPrice(origOrder->getPrice());
          if(qty !=0)
            order->setQuantity(qty);
          else
            order->setQuantity(origOrder->getQuantity());

          if(_context->reqQryOrderStatus(leg1.orderId) != API2::CONSTANTS::RSP_OrderStatus_PENDING){

            if(_context->reqReplaceOrder(risk,leg1.instrument,order,leg1.orderId,_orderPriority))
            {
              _isPendingReplace = true;
              return true;
            }
            else
            {

              DEBUG_MESSAGE(_context->reqQryDebugLog(), "_context->reqReplaceOrder fail");
              DEBUG_VARSHOW(_context->reqQryDebugLog(), "Replace Order Rejected. Reason:", risk);
              return false;
            }
          }
          else{
            DEBUG_MESSAGE(_context->reqQryDebugLog(),"Current state of Original Order is Pending");
            return false;
          }
        }
        DEBUG_MESSAGE(_context->reqQryDebugLog(),"Original Order NULL");
        return false;
      }
      else
      {
        DEBUG_MESSAGE(_context->reqQryDebugLog(), "Order Wrapper Not Set. Terminating strategy");
        _context->reqAddStrategyComment(API2::CONSTANTS::RSP_StrategyComment_STRATEGY_ERROR_STATE);
        _context->reqTerminateStrategy();
        return false;
      }
      return false;

    }
    bool OrderWrapper::cancelOrder(DATA_TYPES::RiskStatus &risk)
    {
      if(!_instrument)
        return false;
      if(_orderLegData.size()!=1){
        std::cout << "Cannot delete Multileg Order" << std::endl;
        return false;
      }
      LegDetail *leg1 = &_orderLegData[0];
      _isReset = false;

      if(!leg1->orderId){
        DEBUG_MESSAGE(_context->reqQryDebugLog(), "OrderWrapper cancel fail OrderId null");
        return false;
      }

      if(_context->reqQryOrderStatus(leg1->orderId) == API2::CONSTANTS::RSP_OrderStatus_PENDING)
      {
        DEBUG_MESSAGE(_context->reqQryDebugLog(),"Current state of Original Order is Pending");
        return false;
      }

      if( _context->reqCancelOrder(risk,leg1->orderId) )
      {
        _isPendingCancel = true;
        return true;
      }
      return false;
    }
    bool OrderWrapper::processConfirmation(OrderConfirmation &confirmation)
    {
      LegDetail *legOrder = getLegOrder(confirmation.getSymbolId());
      if(!legOrder)
      {
        DEBUG_VARSHOW(_context->reqQryDebugLog(),"leg Order Found Null SymbolID:",confirmation.getSymbolId());
        DEBUG_FLUSH(_context->reqQryDebugLog());
      }
      if(confirmation.getOrderStatus() == API2::CONSTANTS::RSP_OrderStatus_CONFIRMED)
        _exchangeOrderId = _context->reqQryExchangeOrderId(legOrder->orderId);

      switch(confirmation.getOrderStatus())
      {
        case API2::CONSTANTS::RSP_OrderStatus_CONFIRMED:
          legOrder->updateOrderWrapper(confirmation.getOrderQuantity(),confirmation.getOrderPrice(),0);
        case API2::CONSTANTS::RSP_OrderStatus_NEW_REJECTED:
          _isPendingNew = false;break;

        case API2::CONSTANTS::RSP_OrderStatus_REPLACED:
          legOrder->updateOrderWrapper(confirmation.getOrderQuantity(),confirmation.getOrderPrice(),0);
        case API2::CONSTANTS::RSP_OrderStatus_REPLACE_REJECTED:
          _isPendingReplace = false;break;

        case API2::CONSTANTS::RSP_OrderStatus_CANCELED:
        case API2::CONSTANTS::RSP_OrderStatus_CANCELED_OF_IOC:
          legOrder->resetOrderWrapper();
        case API2::CONSTANTS::RSP_OrderStatus_CANCEL_REJECTED:
          _isPendingCancel = false;break;

        case API2::CONSTANTS::RSP_OrderStatus_FILLED:
          if(confirmation.getLastFillQuantity() + getLastFilledQuantity() != legOrder->_lastQuantity)//PartiallyFILLed
          {
            legOrder->updateOrderWrapper(getLastQuantity(),legOrder->_lastQuotedPrice,confirmation.getLastFillQuantity());
            break;
          }
          legOrder->resetOrderWrapper();
          break;

      }

      if(strcmp(confirmation.getExchangeOrderId().c_str(),_exchangeOrderId.c_str()))
      {
        DEBUG_MESSAGE(_context->reqQryDebugLog(),"Mismatch in order id");
        DEBUG_VARSHOW(_context->reqQryDebugLog(),"confirmation ExchangeOrderId",confirmation.getExchangeOrderId().c_str());
        DEBUG_VARSHOW(_context->reqQryDebugLog(),"_exchangeOrderId",_exchangeOrderId.c_str());
        return false;
      }
      return true;
    }

    LegDetail &getLegDetail(const DATA_TYPES::SYMBOL_ID &symbolId);

  }
}
